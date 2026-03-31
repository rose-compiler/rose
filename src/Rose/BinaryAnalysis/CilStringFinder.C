#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/CilStringFinder.h>

#include <codecvt>

#include <sageGeneric.h>
#include <Rose/BinaryAnalysis/String.h>


namespace // anonymous
{

using StringCollectorFn = Rose::BinaryAnalysis::CilStringFinder::StringCollectorFn;
using CilStringKinds    = Rose::BinaryAnalysis::CilStringFinder::CilStringKinds;

// \todo replace with functions in ROSE's String.C
std::string readUtf16StringFromHeap(const std::vector<uint8_t>& buf, size_t ofs, size_t maxLen)
{
  if (maxLen == 0)
    return {};

  ASSERT_require((maxLen & 1) == 1);

  std::uint8_t   const* chseq = buf.data() + ofs;
  std::uint8_t   const* chlim = chseq + maxLen;
  std::u16string string16;

  while ((chseq-1) < chlim)
  {
    std::uint16_t lo = (*chseq);
    std::uint16_t hi = (*++chseq);
    std::uint16_t ch = (hi << 8) + lo;

    string16.push_back(ch);
    ++chseq;
  }

  string16.pop_back();

  std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;

  return conv.to_bytes(string16);
}


std::tuple<std::string, size_t, size_t>
decodeUsString_(const std::vector<uint8_t>& buf, size_t ofs)
{
  static constexpr std::uint8_t LEN_2_BYTES = 1 << 7;
  static constexpr std::uint8_t LEN_4_BYTES = LEN_2_BYTES + (1 << 6);

  uint32_t lengthByte = buf.at(ofs);
  uint32_t len = 0;
  uint32_t beg = 0;

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

  return { readUtf16StringFromHeap(buf, beg, len), beg, len };
}

std::string decodeUsString(const std::vector<uint8_t>& buf, size_t ofs)
{
  return std::get<0>(decodeUsString_(buf, ofs));
}


std::string asString(std::uint8_t const* s)
{
  std::string res;

  if (s == nullptr) return res;

  while (*s != 0) { res.push_back(*s); ++s; }

  return res;
}



struct DecoderState
{
  size_t addr = 0;
};

using MetadataRootHandler  = std::function<void(const SgAsmCilMetadataRoot&)>;
using MethodHandler        = std::function<void(const SgAsmCilMethodDef&)>;
using InstructionDecoderFn = std::function<void(const SgAsmCilInstruction&, DecoderState)>;

void forAllInstructions(const SgAsmBlock& blk, InstructionDecoderFn decoder)
{
  DecoderState state;

  for (SgAsmStatement* stmt : blk.get_statementList())
  {
    sg::NotNull<SgAsmCilInstruction> insn = isSgAsmCilInstruction(stmt);

    decoder(*insn, state);
    state = DecoderState{ state.addr + insn->get_size() };
  }
}

void forAllMethods(const SgAsmCilMetadataRoot& root, MethodHandler fn)
{
  using namespace Sawyer::Message;
  using Rose::BinaryAnalysis::Strings::mlog;

  const SgAsmCilMetadataHeap*   metadataHeap = root.get_MetadataHeap();
  if (!metadataHeap)
  {
    mlog[ERROR] << "no metadata heap found."
                << std::endl;
    return;
  }

  const SgAsmCilMethodDefTable* methodDefs = metadataHeap->get_MethodDefTable();
  const bool                    hasMethodDefs = (methodDefs && (methodDefs->get_elements().size()));

  if (!hasMethodDefs)
  {
    mlog[INFO] << "no method definitions found."
               << std::endl;
    return;
  }

  for (sg::NotNull<const SgAsmCilMethodDef> method : methodDefs->get_elements())
    fn(*method);
}


InstructionDecoderFn
cilStringFinder(const SgAsmCilMetadataRoot& root, StringCollectorFn stringCollector)
{
  return
      [usHeap = root.get_UsHeap(), collector = std::move(stringCollector)]
      (const SgAsmCilInstruction& insn, DecoderState) -> void
      {
        if (insn.get_mnemonic() == "ldstr")
        {
          ASSERT_not_null(usHeap);

          // handle strings
          sg::NotNull<const SgAsmOperandList>            oplst   = insn.get_operandList();
          const SgAsmExpressionPtrList&                  offsets = oplst->get_operands();
          sg::NotNull<const SgAsmIntegerValueExpression> expr    = isSgAsmIntegerValueExpression(offsets.at(0));

          const std::uint32_t ref = expr->get_value();
          const std::uint8_t  N   = 24; // 3 * CHAR_BIT;
          const std::uint8_t  tbl = ref >> N;
          const std::uint32_t idx = ref ^ (tbl << N);

          // PP: assuming that ldstr always loads from the USHeap
          ASSERT_require(tbl == 0x70);

          collector(decodeUsString(usHeap->get_Stream(), idx));
        }
      };
}


/// Tests if \p option is set in \p setting.
bool
hasOption(CilStringKinds setting, CilStringKinds option)
{
  // Cast to the underlying type to perform bitwise operations
  std::uint8_t const settingValue = static_cast<std::uint8_t>(setting);
  std::uint8_t const optionValue  = static_cast<std::uint8_t>(option);

  // Check if the bits in option are set in setting
  // This works even if option has multiple bits set
  return (settingValue & optionValue) == optionValue;
}


void
collectUsedStringLiterals(StringCollectorFn collector, const SgAsmCilMetadataRoot& root, CilStringKinds kinds)
{
  using namespace Sawyer::Message;
  using Rose::BinaryAnalysis::Strings::mlog;

  constexpr std::uint8_t CIL_CODE       = 0;
  constexpr std::uint8_t NATIVE_CODE    = 1;
  constexpr std::uint8_t OPTIL_RESERVED = 2;
  constexpr std::uint8_t RUNTIME_CODE   = 3;
  constexpr std::uint8_t CODE_TYPE_MASK = CIL_CODE | NATIVE_CODE | OPTIL_RESERVED | RUNTIME_CODE;

  if (!hasOption(kinds, CilStringKinds::literals))
    return;

  auto methodHandler =
      [ stringCollector=cilStringFinder(root, collector)
      ]
      (const SgAsmCilMethodDef& n) -> void
      {
        // abstract methods
        if (n.get_RVA() == 0) return;

        const bool decodingError = (n.get_bodyState() <= 0);

        // decoding error
        if (decodingError)
        {
          mlog[WARN] << "skipping method '" << n.get_Name_string() << "' with error state " << n.get_bodyState()
                     << std::endl;
          return;
        }

        std::uint8_t const decoderFlags = n.get_ImplFlags() & CODE_TYPE_MASK;

        if (decoderFlags != CIL_CODE)
        {
          mlog[WARN] << "skipping method containing non-CIL code " << n.get_Name_string()
                     << std::endl;
          return;
        }

        const SgAsmBlock* const blk = n.get_body();
        if (blk == nullptr)
        {
          mlog[WARN] << "skipping method with empty code block " << n.get_Name_string()
                     << std::endl;
          return;
        }

        forAllInstructions(*blk, stringCollector);
      };

  forAllMethods(root, methodHandler);
}


void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilAssembly& el)
{
  collector(asString(el.get_Name_string()));
  collector(asString(el.get_Culture_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilAssemblyRef& el)
{
  collector(asString(el.get_Name_string()));
  collector(asString(el.get_Culture_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilEvent& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilField& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilFile& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilGenericParam& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilManifestResource& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilMemberRef& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilMethodDef& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilModule& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilModuleRef& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilParam& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilProperty& el)
{
  collector(asString(el.get_Name_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilExportedType& el)
{
  collector(asString(el.get_TypeName_string()));
  collector(asString(el.get_TypeNamespace_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilTypeDef& el)
{
  collector(asString(el.get_TypeName_string()));
  collector(asString(el.get_TypeNamespace_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilTypeRef& el)
{
  collector(asString(el.get_TypeName_string()));
  collector(asString(el.get_TypeNamespace_string()));
}

void collectStringsFromMetadata(StringCollectorFn collector, const SgAsmCilImplMap& el)
{
  collector(asString(el.get_ImportName_string()));
}

template <class SageAsmCilMetadataTable>
void
collectStringsFromTable(StringCollectorFn collector, const SageAsmCilMetadataTable* table)
{
  if (table == nullptr) return;

  for (sg::NotNull<const typename SageAsmCilMetadataTable::CilMetadataType> elem : table->get_elements())
    collectStringsFromMetadata(collector, *elem);
}

void
collectMetadataStrings(StringCollectorFn collector, const SgAsmCilMetadataRoot& root, CilStringKinds kinds)
{
  using namespace Sawyer::Message;
  using Rose::BinaryAnalysis::Strings::mlog;

  if (!hasOption(kinds, CilStringKinds::metadata))
    return;

  const SgAsmCilMetadataHeap* metadataHeap = root.get_MetadataHeap();
  if (!metadataHeap)
  {
    mlog[ERROR] << "no metadata heap found."
                << std::endl;
    return;
  }

  collectStringsFromTable(collector, metadataHeap->get_AssemblyTable());
  collectStringsFromTable(collector, metadataHeap->get_AssemblyRefTable());
  collectStringsFromTable(collector, metadataHeap->get_EventTable());
  collectStringsFromTable(collector, metadataHeap->get_ExportedTypeTable());
  collectStringsFromTable(collector, metadataHeap->get_FieldTable());
  collectStringsFromTable(collector, metadataHeap->get_FileTable());
  collectStringsFromTable(collector, metadataHeap->get_GenericParamTable());
  collectStringsFromTable(collector, metadataHeap->get_ImplMapTable());
  collectStringsFromTable(collector, metadataHeap->get_ManifestResourceTable());
  collectStringsFromTable(collector, metadataHeap->get_MemberRefTable());
  collectStringsFromTable(collector, metadataHeap->get_MethodDefTable());
  collectStringsFromTable(collector, metadataHeap->get_ModuleTable());
  collectStringsFromTable(collector, metadataHeap->get_ModuleRefTable());
  collectStringsFromTable(collector, metadataHeap->get_ParamTable());
  collectStringsFromTable(collector, metadataHeap->get_PropertyTable());
  collectStringsFromTable(collector, metadataHeap->get_TypeDefTable());
  collectStringsFromTable(collector, metadataHeap->get_TypeRefTable());
}


void forAllMetadataRoots(const SgAsmPEFileHeader& n, MetadataRootHandler fn)
{
  for (const SgAsmGenericSection* gs : n.get_mappedSections())
  {
    if (const SgAsmCliHeader* ch = isSgAsmCliHeader(gs))
    {
      if (const SgAsmCilMetadataRoot* metadata = ch->get_metadataRoot())
        fn(*metadata);
    }
  }
}

void forAllMetadataRoots(const SgBinaryComposite& n, MetadataRootHandler fn)
{
  sg::NotNull<SgAsmGenericFileList> fl = n.get_genericFileList();

  for (sg::NotNull<SgAsmGenericFile> gf : fl->get_files())
  {
    if (SgAsmPEFileHeader* hd = isSgAsmPEFileHeader(gf->get_header(SgAsmExecutableFileFormat::FAMILY_PE)))
      forAllMetadataRoots(*hd, fn);
  }
}

void forAllMetadataRoots(const SgProject& prj, MetadataRootHandler fn)
{
  for (const SgFile* file : prj.get_files())
  {
    if (const SgBinaryComposite* bc = isSgBinaryComposite(file))
      forAllMetadataRoots(*bc, fn);
  }
}

} // anonymous namespace

namespace Rose {
namespace BinaryAnalysis {

void
CilStringFinder::collect(const SgProject& proj, StringCollectorFn collector, CilStringKinds kinds) const
{
  auto stringCollector =
      [collector, kinds]
      (const SgAsmCilMetadataRoot& root) -> void
      {
        collectUsedStringLiterals(collector, root, kinds);
        collectMetadataStrings(collector, root, kinds);
      };

  forAllMetadataRoots(proj, stringCollector);
}

std::vector<std::string>
CilStringFinder::find(const SgProject& proj, CilStringKinds kinds) const
{
  std::vector<std::string> res;

  StringCollectorFn stringCollector =
      [&res](std::string s)->void { res.push_back(std::move(s)); };

  this->collect(proj, stringCollector, kinds);
  return res;
}

}
}

#endif /*ROSE_ENABLE_BINARY_ANALYSIS*/
