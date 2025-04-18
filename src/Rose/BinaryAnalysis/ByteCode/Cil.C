#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ByteCode/Cil.h>
#include <Rose/BinaryAnalysis/Disassembler/Cil.h>

#include <SgAsmBlock.h>
#include <SgAsmCilAssembly.h>
#include <SgAsmCilAssemblyTable.h>
#include <SgAsmCilField.h>
#include <SgAsmCilInstruction.h>
#include <SgAsmCilMemberRef.h>
#include <SgAsmCilMetadata.h>
#include <SgAsmCilMetadataHeap.h>
#include <SgAsmCilMetadataRoot.h>
#include <SgAsmCilMethodDef.h>
#include <SgAsmCilMethodDefTable.h>
#include <SgAsmCilMethodSpec.h>
#include <SgAsmCilModule.h>
#include <SgAsmCilModuleRef.h>
#include <SgAsmCilModuleTable.h>
#include <SgAsmCilTypeDef.h>
#include <SgAsmCilTypeDefTable.h>
#include <SgAsmCilTypeRef.h>
#include <SgAsmInstructionList.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmOperandList.h>

#include <Cxx_GrammarDowncast.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

constexpr bool TRACE_CONSTRUCTION = false;

//...........................................................................................
// Utility functions from testLinearDisassembler3.C
//TODO with them?
//...........................................................................................

struct InstrAddr
{
  Address addr = 0;
};

std::string utf8ToString(const uint8_t* bytes) {
    std::string str{};
    while (*bytes) {
      auto ch = *bytes++;
      str += ch;
    }
    return str;
}

std::string readUtf16String(const std::vector<uint8_t>& buf, size_t ofs, size_t maxLen)
{
  std::string res;
  std::uint8_t const* chseq = buf.data() + ofs; 
  std::uint8_t const* chlim = chseq + maxLen;
  
  while ((chseq+1) < chlim)
  {
    std::uint16_t lo = (*chseq);
    std::uint16_t hi = (*++chseq);
    std::uint16_t ch = (hi << 8) + lo;
    
    ROSE_ASSERT(hi == 0); // \todo decode real UTF16 string
    
    res += char(ch);
    ++chseq;
  }
  
  return res;
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
    os << PrintName{mref->get_Class_object()} << "." << mref->get_Name_string();
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

CilMethod::CilMethod(SgAsmCilMetadataRoot* mdr, SgAsmCilMethodDef* sgMethod, Address va)
  : Method{va}, mdr_{mdr}, sgMethod_{sgMethod}, insns_{nullptr}, code_{nullptr, 0, 0}
{
    insns_ = new SgAsmInstructionList;

    SgAsmBlock* blk = sgMethod_->get_body();
    for (SgAsmStatement* stmt : blk->get_statementList()) {
        auto insn = isSgAsmCilInstruction(stmt);
        ASSERT_not_null(insn);
        insns_->get_instructions().push_back(insn);
    }
}

std::string
CilMethod::name() const {
  return utf8ToString(sgMethod_->get_Name_string());
}

bool
CilMethod::isSystemReserved(const std::string &name) const {
  return CilContainer::isCilSystemReserved(name);
}

const SgAsmInstructionList*
CilMethod::instructions() const {
  return insns_;
}

const Code &
CilMethod::code() const {
  return code_;
}

void
CilMethod::decode(const Disassembler::BasePtr & /*disassembler*/) const {
  std::cerr << "CilMethod::decode():UNIMPLEMENTED\n";
}

void
CilMethod::annotate() {
  for (auto insn : instructions()->get_instructions()) {
    std::string comment{};
    auto cilInsn{isSgAsmCilInstruction(insn)};
    switch (cilInsn->get_kind()) {
      case Cil_call:
      case Cil_callvirt:
      case Cil_newarr:
      case Cil_newobj:
      case Cil_stfld:
      case Cil_stsfld: {
        // metadata token for a methodref, methoddef, or methodspec
        if (auto token = isSgAsmIntegerValueExpression(insn->get_operandList()->get_operands()[0])) {
          if (SgAsmCilMetadata* obj = CilContainer::resolveToken(token, mdr_)) {
            comment = CilClass::objectName(obj, mdr_);
            token->set_comment(comment);
            // Also store the name in the instruction's comment for later convenience in partitioning
            insn->set_comment(comment);
          }
        }
        break;
      }
      default: ;
    }
  }
}

CilClass::CilClass(std::shared_ptr<Namespace> ns, SgAsmCilMetadataRoot* root, const std::string &name, size_t methodBegin, size_t methodLimit)
  : Class{ns,0}, name_{name}, mdr_{root}
{
    SgAsmCilMetadataHeap* metadataHeap = mdr_->get_MetadataHeap();
    ASSERT_not_null(metadataHeap);

    SgAsmCilMethodDefTable* methodDefs = metadataHeap->get_MethodDefTable();
    ASSERT_not_null(methodDefs);

    const std::vector<SgAsmCilMethodDef*>& methods = methodDefs->get_elements();
    methodLimit = std::min(methodLimit, methods.size());

    for (size_t i = methodBegin; i < methodLimit; ++i) {
        SgAsmCilMethodDef* methodDef = methods.at(i);
        ASSERT_not_null(methodDef);

        if (methodDef->get_RVA() == 0) {
            // TODO: double check test file to see if this is an interface (a nop!)
            // std::cerr << "methodDef rva == 0 // abstract\n";
            continue;
        }

        auto method = new CilMethod(mdr_, methodDef, address());
        methods_.push_back(method);

        if (TRACE_CONSTRUCTION) {
          std::cout << "    .method " << method->name() << "\n    {\n";
          for (auto insn : method->instructions()->get_instructions()) {
            std::cout << "       " << insn->toString() << std::endl;
          }
          std::cout << "    }\n";
        }
    }
}

std::string
CilClass::objectName(const SgAsmCilMetadata* obj, SgAsmCilMetadataRoot* mdr)
{
  std::string objName{};
  try {
    switch (obj->variantT()) {
      case V_SgAsmCilField:
        objName += utf8ToString(isSgAsmCilField(obj)->get_Name_string());
        break;
      case V_SgAsmCilMemberRef: {
        auto memberRef = isSgAsmCilMemberRef(obj);
        if (const SgAsmCilMetadata* cls = memberRef->get_Class_object()) {
          // MethodDef, ModuleRef, TypeDef, TypeRef, or TypeSpec
          objName += CilClass::objectName(cls, mdr) + ".";
        }
        objName += utf8ToString(memberRef->get_Name_string());
        break;
      }
      case V_SgAsmCilMethodDef: {
        auto methodDef = isSgAsmCilMethodDef(obj);

        // Search for owning TypeDef
        auto mdh = mdr->get_MetadataHeap();
        auto typeDefTable = mdh->get_TypeDefTable();
        for (SgAsmCilTypeDef* typeDef: typeDefTable->get_elements()) {
          if (isSgAsmCilTypeDef(typeDef)) {
            // Ignore <Module> which doesn't extend any type/class
            if (typeDef->get_Extends()) {
              auto method = typeDef->get_MethodList_object(methodDef);
              if (/*ignore module*/typeDef->get_Extends() && method == methodDef) {
                objName += utf8ToString(typeDef->get_TypeNamespace_string()) + "." + utf8ToString(typeDef->get_TypeName_string()) + ".";
                break;
              }
            }
          }
        }
        objName += utf8ToString(methodDef->get_Name_string());
        ASSERT_require(objName.size() > 0);
        break;
      }
      case V_SgAsmCilMethodSpec: {
        auto methodSpec = isSgAsmCilMethodSpec(obj);
        // MethodDef or MemberRef
        uint32_t index = methodSpec->get_Method();
        if (index > 0) {
          auto mdh = mdr->get_MetadataHeap();
          SgAsmCilMetadata* specObj = mdh->get_CodedMetadataNode(index, SgAsmCilMetadataHeap::e_ref_method_def_or_ref);
          objName += CilClass::objectName(specObj, mdr);
        } else {
          mlog[WARN] << "index out_of_range: must be > 0: in CilClass::objectName for " << obj->class_name() << "\n";
        }
        break;
      }
      case V_SgAsmCilModuleRef: {
        auto moduleRef = isSgAsmCilModuleRef(obj);
        objName += utf8ToString(moduleRef->get_Name_string());
        break;
      }
      case V_SgAsmCilTypeDef: {
        auto typeDef = isSgAsmCilTypeDef(obj);
        objName += utf8ToString(typeDef->get_TypeNamespace_string()) + "." + utf8ToString(typeDef->get_TypeName_string());
        break;
      }
      case V_SgAsmCilTypeRef: {
        auto typeRef = isSgAsmCilTypeRef(obj);
        objName += utf8ToString(typeRef->get_TypeNamespace_string()) + "." + utf8ToString(typeRef->get_TypeName_string());
        break;
      }
      case V_SgAsmCilTypeSpec:
        // Only has a signature
        break;
      default: ;
    }
  }
  catch(const std::out_of_range& e) {
    mlog[WARN] << "out_of_range exception: " << e.what() << " in CilClass::objectName for " << obj->class_name() << "\n";
    objName = "UNKNOWN";
  }

  return objName;
}

void CilClass::dump()
{
  mlog[WARN] << "CilClass::dump() unimplemented\n";
}

CilNamespace::CilNamespace(SgAsmCilMetadataRoot* /*root*/, const std::string &name)
  : name_{name}
{
}

std::string
CilNamespace::name() const {
    return name_;
}

CilContainer::CilContainer(SgAsmCilMetadataRoot* root) : mdr_{root} {
    SgAsmCilMetadataHeap* metadataHeap = mdr_->get_MetadataHeap();
    ASSERT_not_null(metadataHeap);
  
    SgAsmCilTypeDefTable* typedefs = metadataHeap->get_TypeDefTable();
    ASSERT_not_null(typedefs);
  
    const std::uint8_t* thisNamespace{nullptr};
    const std::uint8_t* lastNamespace{nullptr};
    const std::vector<SgAsmCilTypeDef*>& typeDefElements = typedefs->get_elements();

    // The first row of the TypeDef table represents the pseudo class that acts as parent
    // for functions and variables defined at module scope. Skip it for now (WARNING, be careful with names for i==0).
    for (size_t i = 1; i < typeDefElements.size(); ++i) {
        const SgAsmCilTypeDef* sgTypeDef = typeDefElements.at(i);
        ASSERT_not_null(sgTypeDef);
    
        thisNamespace = sgTypeDef->get_TypeNamespace_string();
        if (lastNamespace != thisNamespace) {
            namespaces_.push_back(std::make_shared<CilNamespace>(mdr_, utf8ToString(sgTypeDef->get_TypeNamespace_string())));

            if (TRACE_CONSTRUCTION) {
              if (lastNamespace) std::cout << "}" << std::endl;
              std::cout << "\n.namespace " << namespaces().back()->name() << "\n{";
            }
            lastNamespace = thisNamespace;
        }
      
        if (TRACE_CONSTRUCTION) {
          // Appears that all typedefs with i>0 are classes (".class")
          std::cout << "\n  .class " << sgTypeDef->get_TypeName_string() << "\n  {" << std::endl;
        }

        // Methods of class
        size_t numMethods = metadataHeap->get_MethodDefTable()->get_elements().size();
        size_t methodBegin = sgTypeDef->get_MethodList()-1;
        size_t methodLimit = (i+1 < typeDefElements.size() ? typeDefElements.at(i+1)->get_MethodList()-1 : numMethods);

        // Construct class
        std::string typeName = namespaces().back()->name() + ".";
        typeName += utf8ToString(sgTypeDef->get_TypeName_string());
        auto cilClass = std::make_shared<CilClass>(namespaces_.back(), mdr_, typeName, methodBegin, methodLimit);
        namespaces_.back()->append(cilClass);
    }
}

std::string
CilContainer::name() const {
  return "CilContainer::name():UNIMPLEMENTED";
}

bool
CilContainer::isSystemReserved(const std::string &name) const {
  return isCilSystemReserved(name);
}

bool
CilContainer::isCilSystemReserved(const std::string &name) {
  if (name.substr(0,6)=="System" || name.substr(0,9)=="Microsoft") {
    return true;
  }
  return false;
}

SgAsmCilMetadata*
CilContainer::resolveToken(SgAsmIntegerValueExpression* token, SgAsmCilMetadataRoot* mdr)
{
  SgAsmCilMetadata* obj{nullptr};

  uint32_t value = static_cast<uint32_t>(token->get_value());
  uint32_t kind =  (0xff000000 & value) >> 24;
  uint32_t index = 0x00ffffff & value;
  auto mdh = mdr->get_MetadataHeap();

  if (index > 0) {
    obj = mdh->get_MetadataNode(index, static_cast<SgAsmCilMetadataHeap::TableKind>(kind));
  }

  return obj;
}

void
CilContainer::printAssemblies(std::ostream& os) const
{
  SgAsmCilMetadataHeap* metadataHeap = mdr_->get_MetadataHeap();
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
CilContainer::printMethods(std::ostream& os, size_t beg, size_t lim) const
{
  constexpr std::uint8_t CIL_CODE       = 0;
  constexpr std::uint8_t NATIVE_CODE    = 1;
  constexpr std::uint8_t OPTIL_RESERVED = 2;
  constexpr std::uint8_t RUNTIME_CODE   = 3;
  constexpr std::uint8_t CODE_TYPE_MASK = CIL_CODE | NATIVE_CODE | OPTIL_RESERVED | RUNTIME_CODE;

  SgAsmCilMetadataHeap* metadataHeap = mdr_->get_MetadataHeap();
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
#if 1
      ASSERT_require(decoderFlags == CIL_CODE);
      
      for (SgAsmStatement* stmt : blk->get_statementList()) {
          if (TRACE_CONSTRUCTION) {
            std::cout << "   ... decode stmt: " << stmt << std::endl;
          }
      }
    }
#else
    for (SgAsmStatement* stmt : blk->get_statementList()) {
      if (auto insn = isSgAsmInstruction(stmt)) {
        if (TRACE_CONSTRUCTION) {
          std::cout << "... " << insn->toString() << std::endl;
        }
      }
      
    }
#endif
    
    if (TRACE_CONSTRUCTION) {
      os << "    }\n" << std::endl;
    }
  }
}

void
CilContainer::printModules(std::ostream& os) const
{
  SgAsmCilMetadataHeap* metadataHeap = mdr_->get_MetadataHeap();
  ASSERT_not_null(metadataHeap);
  
  SgAsmCilModuleTable* modules = metadataHeap->get_ModuleTable();
  ASSERT_not_null(modules);
  
  for (SgAsmCilModule* mod : modules->get_elements()) {
    ASSERT_not_null(mod);
    os << ".module " << mod->get_Name_string()
       << " // GUID"
       << std::endl;    
  }
}

void
CilContainer::printTypeDefs(std::ostream& os) const
{
  SgAsmCilMetadataHeap* metadataHeap = mdr_->get_MetadataHeap();
  ASSERT_not_null(metadataHeap);
  
  SgAsmCilTypeDefTable* typedefs = metadataHeap->get_TypeDefTable();
  ASSERT_not_null(typedefs);
  
  const std::uint8_t* lastNamespace = nullptr;
  const std::vector<SgAsmCilTypeDef*>& typeDefElements = typedefs->get_elements();

  for (size_t i = 0; i < typeDefElements.size(); ++i)
  {
    const SgAsmCilTypeDef* td = typeDefElements.at(i);
    ASSERT_not_null(td);
    
    if (i)
    {
      const std::uint8_t* thisNamespace = td->get_TypeNamespace_string();
      
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
    size_t lim  = (i+1 < typeDefElements.size() ? typeDefElements.at(i+1)->get_MethodList()-1 : numMethods);
    
    printMethods(os, beg, lim);    

    if (i) os << "  }" << std::endl;     
  }
  
  if (lastNamespace) os << "}" << std::endl;
}

} // namespace
} // namespace
} // namespace

#endif
