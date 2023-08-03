#include <rose.h>
#include <rosePublicConfig.h>

#include <codecvt>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/CommandLine.h>

#include <Sawyer/Message.h>

using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;

namespace // anonymous
{

// Tool-specific command-line settings
struct Settings {
    rose_addr_t startVa;
    rose_addr_t alignment;
    bool runSemantics;
    Settings(): startVa(0), alignment(1), runSemantics(false) {}
};

// Build a command line parser without running it
Sawyer::CommandLine::Parser
buildSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    std::string purpose = "disassembles files one address at a time";
    std::string description =
        "This program is a very simple disassembler that tries to disassemble in instruction at each executable "
        "address, one instruction after the next.";

    SwitchGroup switches("Tool-specific switches");
    switches.name("tool");

    switches.insert(Switch("start")
                    .argument("virtual-address", nonNegativeIntegerParser(settings.startVa))
                    .doc("Address at which disassembly will start.  The default is to start at the lowest mapped "
                         "address."));
    switches.insert(Switch("alignment")
                    .argument("align", nonNegativeIntegerParser(settings.alignment))
                    .doc("Alignment for instructions.  The default is 1 (no alignment).  Values larger than one will "
                         "cause each candidate address to be rounded up to the next multiple of @v{align}.  If this "
                         "rounding up causes addresses after a valid instruction to be skipped then a warning is printed. "
                         "No warning is printed if the alignment skips addresses after a disassembly failure."));
    switches.insert(Switch("semantics")
                    .intrinsicValue(true, settings.runSemantics)
                    .doc("Run semantics for each basic block. This is only useful to debug instruction semantics."));
    switches.insert(Switch("no-semantics")
                    .key("semantics")
                    .intrinsicValue(false, settings.runSemantics)
                    .hidden(true));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen_names}");
    parser.errorStream(mlog[FATAL]);
    parser.with(Rose::CommandLine::genericSwitches());
    parser.with(switches);

    return parser;
}


std::string readUtf16String(const std::vector<uint8_t>& buf, size_t ofs, size_t maxLen)
{
  std::uint8_t   const* chseq = buf.data() + ofs; 
  std::uint8_t   const* chlim = chseq + maxLen;
  std::u16string string16;
  
  while ((chseq+1) < chlim)
  {
    std::uint16_t lo = (*chseq);
    std::uint16_t hi = (*++chseq);
    std::uint16_t ch = (hi << 8) + lo;
    
    string16.push_back(ch);    
    ++chseq;
  }
  
  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
  
  return conv.to_bytes(string16);
}

std::string decodeUsString(const std::vector<uint8_t>& buf, size_t ofs)
{
  static constexpr std::uint8_t LEN_2_BYTES = 1 << 7;
  static constexpr std::uint8_t LEN_4_BYTES = LEN_2_BYTES + (1 << 6);
  
  uint32_t lengthByte = buf.at(ofs);
  uint32_t len = 0;
  uint8_t  beg = 0;
  
  if ((lengthByte & LEN_4_BYTES) == LEN_4_BYTES)
  {
    beg = ofs+4;
    len = (  ((lengthByte ^ LEN_4_BYTES) << 24)
          +  (uint32_t(buf.at(ofs+1)) << 16)
          +  (uint32_t(buf.at(ofs+2)) << 8)
          +  (uint32_t(buf.at(ofs+3)))
          );
  }
  else if ((lengthByte & LEN_2_BYTES) == LEN_2_BYTES)
  {
    beg = ofs+2;
    len = ((lengthByte ^ LEN_2_BYTES) << 8) + buf.at(ofs+1);
  }
  else
  {
    beg = ofs+1;
    len = lengthByte;
  }
    
  return readUtf16String(buf, beg, len);
}



const SgAsmCilMetadata*
lookupNode(const SgAsmCilMetadataHeap* n, std::uint32_t ref) 
{
  const std::uint8_t      N   = 24; // 3 * CHAR_BIT;
  const std::uint8_t      tbl = ref >> N;
  const std::uint32_t     idx = ref ^ (tbl << N);
  const SgAsmCilMetadata* res = nullptr;
  
  if (idx == 0) return res;  
  
  return n->get_MetadataNode(idx, static_cast<SgAsmCilMetadataHeap::TableKind>(tbl));
}

          
struct DecoderState
{
  size_t addr = 0;
};

struct InstrAddr
{
  size_t addr = 0;
};

std::ostream& operator<<(std::ostream& os, InstrAddr addr)
{
  std::stringstream fmt;
  
  fmt << std::setfill('0') << std::setw(4) << std::hex << addr.addr;  
  os << "il_" << fmt.str();
  return os;
}

template <class T>
struct PrintValue
{
  T val;
};

template <class T>
std::ostream& operator<<(std::ostream& os, PrintValue<T> iv)
{
  return os << iv.val;
}


PrintValue<std::int64_t>
intValue(const SgAsmIntegerValueExpression* expr)
{
  ASSERT_not_null(expr);
  
  return PrintValue<std::int64_t>{expr->get_signedValue()};
}

PrintValue<std::uint64_t>
uintValue(const SgAsmIntegerValueExpression* expr)
{
  ASSERT_not_null(expr);
  
  return PrintValue<std::uint64_t>{expr->get_value()};
}

DecoderState 
x86Decoder(std::ostream& os, SgAsmCilMetadataHeap* heap, SgAsmStatement* stmt, DecoderState state)
{
  SgAsmX86Instruction* insn = isSgAsmX86Instruction(stmt);
  ROSE_ASSERT(insn);

  os << "  @" << InstrAddr{state.addr} << " " << insn->get_mnemonic() 
     << std::endl;

  return DecoderState{ state.addr + insn->get_size() }  ;
}

struct PrintName
{
  const SgAsmCilMetadata* n = nullptr;
};

std::ostream&
operator<<(std::ostream& os, PrintName pn)
{
  if (const SgAsmCilTypeDef* ty = isSgAsmCilTypeDef(pn.n))
    os << ty->get_TypeName_string();
  else if (const SgAsmCilMethodDef* me = isSgAsmCilMethodDef(pn.n))
    os << me->get_Name_string();
  else if (const SgAsmCilField* fld = isSgAsmCilField(pn.n))
    os << fld->get_Name_string();
  else if (const SgAsmCilModuleRef* mod = isSgAsmCilModuleRef(pn.n))
    os << mod->get_Name_string();
  else if (const SgAsmCilTypeRef* tr = isSgAsmCilTypeRef(pn.n))
    os << tr->get_TypeName_string();
  else if (const SgAsmCilMemberRef* mref = isSgAsmCilMemberRef(pn.n))
    os << PrintName{mref->get_Class_object()} << "::" << mref->get_Name_string();
  else if (/*const SgAsmCilTypeSpec* tr =*/ isSgAsmCilTypeSpec(pn.n))
  {
    // \todo
    // typeSpec points to a Blob with a type encoding
    // e.g.,  https://github.com/dotnet/runtime/blob/main/docs/design/specs/Ecma-335-Augments.md
    os << '[' << "typeSpec encoded blob" << ']';
  }
  else if (pn.n)
    os << '[' << typeid(*pn.n).name() << " ??]";
  else
    os << '[' << "null" << ']';
  
  return os;
}

PrintName
metadataToken(const SgAsmCilMetadataHeap* heap, const SgAsmIntegerValueExpression* expr)
{
  return PrintName{expr ? lookupNode(heap, expr->get_value()) : nullptr};
}

PrintName
metadataToken(const SgAsmCilMetadataHeap* heap, const SgAsmExpression* expr)
{
  return metadataToken(heap, isSgAsmIntegerValueExpression(expr));
}

DecoderState 
cilDecoder(std::ostream& os, SgAsmCilMetadataHeap* heap, SgAsmStatement* stmt, DecoderState state)
{
  SgAsmCilInstruction* insn = isSgAsmCilInstruction(stmt);
  ROSE_ASSERT(insn);
  
  std::string                   memn = insn->get_mnemonic();
  const size_t                  nextInsn = state.addr + insn->get_size();   
  const SgAsmOperandList*       oplst    = insn->get_operandList();
  const SgAsmExpressionPtrList& offsets  = oplst->get_operands();
  ROSE_ASSERT(oplst);
  
  os << "      " << InstrAddr{state.addr} << ": " << memn << std::flush; 
     
  if (memn == "switch")
  {
    static constexpr int TARGETS_PER_LINE = 4;
    
    os << "\n        ( ";
    for (std::size_t i = 1; i < offsets.size(); ++i)
    {
      const SgAsmIntegerValueExpression* expr = isSgAsmIntegerValueExpression(offsets.at(i));
      ROSE_ASSERT(expr);
      
      if (i != 1) os << ((i-1) % TARGETS_PER_LINE ? ", " : ",\n          ");                          
      
      os << InstrAddr{nextInsn + expr->get_signedValue()};
    }
    os << "\n        )" << std::endl;
  }
  else if (memn == "break")
  {
    // nothing to do
  }
  else if (memn == "unbox")
  {
    // handle value type
    // \todo UNTESTED
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if (memn == "newarr")
  {
    // handle etype token
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if (memn == "ldtoken")
  {
    // handle token code
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if (  (memn == "box")       || (memn == "cpobj")  || (memn == "initobj") || (memn == "ldelem") 
          || (memn == "ldobj")     || (memn == "sizeof") || (memn == "stelem")  || (memn == "stobj")
          || (memn == "unbox_any")
          )
  {
    // handle type token
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if ((memn == "castclass") || (memn == "isinst") || (memn == "ldelema") || (memn == "mkrefany"))
  {
    // handle class token
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if ((memn == "refanyval"))
  {
    // handle type token?
    // \todo UNTESTED
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if ((memn == "constrained"))
  {
    // handle this type token?
    // \todo UNTESTED
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if (memn.rfind("leave") == 0)
  {
    // should this be a InstrAddr?
    os << " " << intValue( isSgAsmIntegerValueExpression(offsets.at(0)) );
  }
  else if (  (memn == "ldfld") || (memn == "ldflda") || (memn == "ldsfld")  || (memn == "ldsflda")
          || (memn == "stfld") || (memn == "stsfld")
          )
  {
    //~ // handle field token
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if (memn == "ldstr") 
  {
    // handle string
    const SgAsmIntegerValueExpression* expr = isSgAsmIntegerValueExpression(offsets.at(0));
    ROSE_ASSERT(expr);

    const std::uint32_t     ref = expr->get_value();    
    const std::uint8_t      N   = 24; // 3 * CHAR_BIT;
    const std::uint8_t      tbl = ref >> N;
    const std::uint32_t     idx = ref ^ (tbl << N);    
    
    // PP: assuming that ldstr always loads from the USHeap
    ROSE_ASSERT(tbl == 0x70);
    const std::vector<std::uint8_t>& usStream = isSgAsmCilMetadataRoot(heap->get_parent())->get_UsHeap()->get_Stream();
    
    os << " \"" << decodeUsString(usStream, idx) << "\"";
  }
  else if (memn.front() == 'b') // all non-branch mnemonics b.* are handled earlier
  {
    const SgAsmIntegerValueExpression* expr = isSgAsmIntegerValueExpression(offsets.at(0));
    ROSE_ASSERT(expr);
    
    os << " " << InstrAddr{nextInsn + expr->get_signedValue()};
  }
  else if (  (memn == "call")      || (memn == "callvirt") || (memn == "jmp") || (memn == "ldftn")
          || (memn == "ldvirtftn") || (memn == "newobj")
          )
  {
    // handle method token
    // \note newobj is listed as carrying a ctor token. 
    //       This seems to be the same as a method token.
    os << " " << metadataToken(heap, offsets.at(0));
  }
  else if ((memn == "ldc_i4") || (memn == "ldc_i4_s") || (memn == "ldc_i8"))
  {
    os << " " << intValue( isSgAsmIntegerValueExpression(offsets.at(0)) );
  }
  else if (  (memn == "ldarg") || (memn == "ldarg_s") || (memn == "ldarga") || (memn == "ldarga_s")
          || (memn == "ldloc") || (memn == "ldloc_s") || (memn == "ldloca") || (memn == "ldloca_s")
          || (memn == "starg") || (memn == "starg_s") || (memn == "stloc")  || (memn == "stloc_s")
          || (memn == "no")    || (memn == "unaligned") 
          )
  {
    os << " " << uintValue( isSgAsmIntegerValueExpression(offsets.at(0)) );
  }

  os << std::endl;
  return DecoderState{ nextInsn };
}

void
printAssemblies(std::ostream& os, const SgAsmCilMetadataRoot* n)
{
  SgAsmCilMetadataHeap* metadataHeap = n->get_MetadataHeap();
  ASSERT_not_null(metadataHeap);
  
  SgAsmCilAssemblyTable* assemblies = metadataHeap->get_AssemblyTable();
  ASSERT_not_null(assemblies);
  
  for (SgAsmCilAssembly* assembly : assemblies->get_elements())
  {
    ASSERT_not_null(assembly);
    os << ".assembly " << assembly->get_Name_string()
       << "\n{"
       << "\n  // Culture: " << assembly->get_Culture_string()
       << "\n  // Flags: " << std::hex << assembly->get_Flags() << std::dec
       << "\n  // Rev: " << assembly->get_RevisionNumber() 
       << "\n  // Build#: " << assembly->get_BuildNumber() 
       << "\n  .ver " << assembly->get_MajorVersion() << ':' << assembly->get_MinorVersion()       
       << "\n  .hash 0x" << std::hex << assembly->get_HashAlgId() << std::dec
       << "\n  // TODO"
       << "\n}"
       << std::endl;    
  }
}

void
printModules(std::ostream& os, const SgAsmCilMetadataRoot* n)
{
  SgAsmCilMetadataHeap* metadataHeap = n->get_MetadataHeap();
  ASSERT_not_null(metadataHeap);
  
  SgAsmCilModuleTable* modules = metadataHeap->get_ModuleTable();
  ASSERT_not_null(modules);
  
  for (SgAsmCilModule* mod : modules->get_elements())
  {
    ASSERT_not_null(mod);
    os << ".module " << mod->get_Name_string()
       << " // GUID"
       << std::endl;    
  }
}


void
printMethods(std::ostream& os, const SgAsmCilMetadataRoot* n, size_t beg = 0, size_t lim = std::numeric_limits<size_t>::max())
{
  using AsmDecoderFn = std::function<DecoderState(std::ostream&, SgAsmCilMetadataHeap*, SgAsmStatement*, DecoderState)>;
  
  constexpr std::uint8_t CIL_CODE       = 0;
  constexpr std::uint8_t NATIVE_CODE    = 1;
  constexpr std::uint8_t OPTIL_RESERVED = 2;
  constexpr std::uint8_t RUNTIME_CODE   = 3;
  constexpr std::uint8_t CODE_TYPE_MASK = CIL_CODE | NATIVE_CODE | OPTIL_RESERVED | RUNTIME_CODE;

  SgAsmCilMetadataHeap* metadataHeap = n->get_MetadataHeap();
  ASSERT_not_null(metadataHeap);
  
  SgAsmCilMethodDefTable* methodDefs = metadataHeap->get_MethodDefTable();
  ASSERT_not_null(methodDefs);
  
  const std::vector<SgAsmCilMethodDef*>& methods = methodDefs->get_elements();
  
  lim = std::min(lim, methods.size());
  
  for (size_t i = beg; i < lim; ++i)
  {
    SgAsmCilMethodDef* methodDef = methods.at(i);     
    ASSERT_not_null(methodDef);  
    os << "    .method " << methodDef->get_Name_string();
      
    std::uint32_t rva = methodDef->get_RVA();
    
    if (rva == 0)
    {
      os << " = 0 // abstract" << std::endl;
      continue;
    }
    
    os << std::endl;
    
    std::uint8_t decoderFlags = methodDef->get_ImplFlags() & CODE_TYPE_MASK;
    ROSE_ASSERT(decoderFlags == CIL_CODE || decoderFlags == NATIVE_CODE);
    
    os << "    {\n" 
       << "      // " << (decoderFlags == CIL_CODE ? "CIL code" : "Native code") << '\n'
       << "      // method begins at 0x" << std::hex << (rva) << std::dec << '\n'
       //~ << "      // header size = " << int(mh.headerSize()) << " (" << (mh.tiny() ? "tiny": "fat") << ")\n"
       //~ << "      // code size " << codeLen << " (0x" << std::hex << codeLen << std::dec << ")\n"
       << "      .entrypoint\n" 
       << "      .maxstack " << methodDef->get_stackSize() << '\n'
       << "      .localsinit " << methodDef->get_initLocals() << '\n'
       << std::flush;           
        
       
    if (SgAsmBlock* blk = methodDef->get_body())
    {    
      AsmDecoderFn decoder = decoderFlags == CIL_CODE ? cilDecoder : x86Decoder;
      DecoderState state;
      
      for (SgAsmStatement* stmt : blk->get_statementList())
        state = decoder(os, metadataHeap, stmt, state);
    }
    
    os << "    }\n" << std::endl;
  }
}

void
printTypeDefs(std::ostream& os, const SgAsmCilMetadataRoot* n)
{
  SgAsmCilMetadataHeap* metadataHeap = n->get_MetadataHeap();
  ASSERT_not_null(metadataHeap);
  
  SgAsmCilTypeDefTable* typedefs = metadataHeap->get_TypeDefTable();
  ASSERT_not_null(typedefs);
  
  const std::uint8_t* lastNamespace = nullptr;
  const std::vector<SgAsmCilTypeDef*>& tydefs = typedefs->get_elements();

  for (size_t i = 0; i < tydefs.size(); ++i)
  {
    const SgAsmCilTypeDef* td = tydefs.at(i);
    ASSERT_not_null(td);
    
    if (i)
    {
      const std::uint8_t*    thisNamespace = td->get_TypeNamespace_string();
      
      if (lastNamespace != thisNamespace)
      {
        if (lastNamespace) os << "}" << std::endl;
        
        os << ".namespace " << thisNamespace << "\n{\n";
        
        lastNamespace = thisNamespace;
      }
      
      os << "\n  .class " << td->get_TypeName_string() 
         << "\n  {"
         << std::endl;     
    }
    
    size_t numMethods = metadataHeap->get_MethodDefTable()->get_elements().size();
    size_t beg = td->get_MethodList()-1;
    size_t lim  = (i+1 < tydefs.size() ? tydefs.at(i+1)->get_MethodList()-1 : numMethods);
    
    printMethods(os, n, beg, lim);    

    if (i) os << "  }" << std::endl;     
  }
  
  if (lastNamespace) os << "}" << std::endl;
}


void forAllMetadataRoots(const SgAsmPEFileHeader& n, std::function<void(const SgAsmCilMetadataRoot*)> fn)
{
  for (const SgAsmGenericSection* gs : n.get_mapped_sections())
  {
    if (const SgAsmCliHeader* ch = isSgAsmCliHeader(gs)) 
    {
      SgAsmCilMetadataRoot* metadata = ch->get_metadataRoot();
      ROSE_ASSERT(metadata);
      
      fn(metadata);
    } 
  }
}

void forAllMetadataRoots(const SgBinaryComposite& n, std::function<void(const SgAsmCilMetadataRoot*)> fn)
{
  SgAsmGenericFileList* fl = n.get_genericFileList();
  ASSERT_not_null(fl);
       
  for (SgAsmGenericFile* gf : fl->get_files())
  {
    ASSERT_not_null(gf);
    
    if (SgAsmPEFileHeader* hd = isSgAsmPEFileHeader(gf->get_header(SgAsmExecutableFileFormat::FAMILY_PE)))
      forAllMetadataRoots(*hd, fn);
  }
}

void forAllMetadataRoots(const SgProject& prj, std::function<void(const SgAsmCilMetadataRoot*)> fn)
{
  for (const SgFile* file : prj.get_files())
  {
    if (const SgBinaryComposite* bc = isSgBinaryComposite(file))
      forAllMetadataRoots(*bc, fn);
  }
}

} // anonymous namespace


int main(int argc, char *argv[])
{
    constexpr bool GEN_DOT_FILES = false;
    
    ROSE_INITIALIZE;
    Rose::Diagnostics::initAndRegister(&::mlog, "tool");

    // Build command line parser (without any knowledge of engine settings), choose the appropriate partitioner engine type, and
    // parse and apply the command-line to the settings, engine, etc. using the appropriately modified command-line parser.
    Settings settings;
    auto parser = buildSwitchParser(settings);
    P2::Engine::Ptr engine = P2::Engine::forge(argc, argv, parser/*in,out*/);
    mlog[INFO] <<"using the " <<engine->name() <<" partitioning engine\n";
    std::vector<std::string> specimenNames = parser.parse(argc, argv).apply().unreachedArgs();

    // Load the specimen as raw data or an ELF or PE container
    /* MemoryMap::Ptr map = */ engine->loadSpecimens(specimenNames);

    SgProject* p = SageInterface::getProject();
    ASSERT_not_null(p);
    
    if (GEN_DOT_FILES)
      generateDOT(p, specimenNames.front());
           
    forAllMetadataRoots( *p, 
                         [os = std::ref(std::cout)] 
                         (const SgAsmCilMetadataRoot* root) mutable -> void 
                         {
                           printAssemblies(os, root);
                           printModules(os, root);
                           printTypeDefs(os, root);                        
                         }
                       );

    exit(0);
}
