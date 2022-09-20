#include <rose.h>
#include <rosePublicConfig.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/TraceSemantics.h>

#include <map>
#include <Sawyer/Assert.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>
#include <string>
#include <cinttypes> /* for uintptr_t and PRIxxx format specifiers*/

using namespace Rose;
using namespace Rose::BinaryAnalysis::InstructionSemantics;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

// DQ (12/16/2021): Added to support use of STL types.
using namespace std;

Sawyer::Message::Facility mlog;


namespace // anonymous
{

// Convenient struct to hold settings from the command-line all in one place.
struct Settings {
    rose_addr_t startVa;
    rose_addr_t alignment;
    bool runSemantics;
    Settings(): startVa(0), alignment(1), runSemantics(false) {}
};

// Describe and parse the command-line

vector<string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings)
{
    using namespace Sawyer::CommandLine;

    std::string purpose = "disassembles files one address at a time";
    std::string description =
        "This program is a very simple disassembler that tries to disassemble in instruction at each executable "
        "address, one instruction after the next.";

    // The parser is the same as that created by Engine::commandLineParser except we don't need any partitioning switches since
    // this tool doesn't partition.
    Parser parser;
    parser
        .purpose(purpose)
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis",
             "@prop{programName} [@v{switches}] @v{specimen_names}")
        .doc("Description", description)
        .doc("Specimens", engine.specimenNameDocumentation())
        .with(engine.engineSwitches())
        .with(engine.loaderSwitches())
        .with(engine.disassemblerSwitches());

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

    return parser.with(switches).parse(argc, argv).apply().unreachedArgs();
}


SgAsmCilMetadataRoot*
obtainMetaDataRoot()
{
  SgAsmCilMetadataRoot* res = nullptr;
  VariantVector         vv{V_SgAsmCilMetadataRoot};
  std::vector<SgNode*>  all = NodeQuery::queryMemoryPool(vv);

  ROSE_ASSERT(all.size() > 0);

  if (all.size() > 1)
    ::mlog[ERROR] << "more than one SgAsmCilMetadataRoot objects." << std::endl;

  res = isSgAsmCilMetadataRoot(all.front());

#if 0
// DQ (12/23/2021): After discussion with Robb, here is the way to compute the constant value.
// Need to compute the value 0x00400000
  SgAsmInterpretation* interpretation = engine.interpretation();

// Find the
  SgAsmGenericList* headerList = interpretation.get_headers();

// Look for the SgAsmPEFileHeader
  SgAsmPEFileHeader* peFileHeader = (found in headerList)

// Look for SgAsmCilHeader
  SgAsmCilHeader* cilheader = ...

// look for SgAsmCilMetadataRoot
  res = /* connection from CilHeader to MetadataRoot object is missing */;

// Should generate constant: 0x00400000
  //~ rose_addr_t base_va = PeFileHeader->get_base_va();

  //~ va = base_va + rvaList[0];
#endif
  ASSERT_not_null(res);
  return res;
}

std::string
getString(const SgAsmCilUint8Heap& heap, size_t idx)
{
  std::string          res;
  std::vector<uint8_t> data = heap.get_Stream();

  while (char c = data.at(idx))
  {
    res += c;
    ++idx;
  }

  return res;
}

vector<uint32_t>
generateRVAs(SgAsmCilMetadataRoot* n)
{
  vector<uint32_t> res;

  printf ("Generate the RVAs for each method: \n");
  SgAsmCilMetadataHeap* metadataHeap = n->get_MetadataHeap();
  SgAsmCilUint8Heap*    stringHeap = n->get_StringHeap();
  ASSERT_not_null(metadataHeap);
  ASSERT_not_null(stringHeap);

  for (SgAsmCilMethodDef* methodDef : metadataHeap->get_MethodDef())
  {
    ASSERT_not_null(methodDef);
    std::cerr << "Method at StringHeap[" << methodDef->get_Name()
              << "] = " << getString(*stringHeap, methodDef->get_Name())
              << std::endl;

    res.push_back(methodDef->get_RVA());
    
    if (SgAsmBlock* blk = methodDef->get_body())
    {    
      for (SgAsmStatement* stmt : blk->get_statementList())
        if (SgAsmCilInstruction* insn = isSgAsmCilInstruction(stmt))
          std::cout << insn->get_mnemonic() << std::endl;
    }
  }

  return res;
}
} // anonymous namespace


int main(int argc, char *argv[])
{
    ROSE_INITIALIZE;

#if 1
    printf ("In TOP of main() \n");
#endif

    Diagnostics::initAndRegister(&::mlog, "tool");

    // Parse the command-line
    P2::Engine engine;
    Settings settings;
    std::vector<std::string> specimenNames = parseCommandLine(argc, argv, engine, settings);

    // Load the specimen as raw data or an ELF or PE container
    MemoryMap::Ptr map = engine.loadSpecimens(specimenNames);
#if 0
    map->dump(::mlog[INFO]);
    map->dump(std::cout);
#endif

    AsmUnparser unparser;
    Disassembler::Base::Ptr disassembler = engine.obtainDisassembler();
    // Obtain an unparser suitable for this disassembler
    unparser.set_registers(disassembler->registerDictionary());


 // DQ (12/18/2021): See if we can comment this out, I think we are not using it.
 // Build semantics framework; only used when settings.runSemantics is set
    BaseSemantics::Dispatcher::Ptr dispatcher;
#if 0
    if (settings.runSemantics) {
        BaseSemantics::RiscOperators::Ptr ops = SymbolicSemantics::RiscOperators::instance(disassembler->registerDictionary());
        ops = TraceSemantics::RiscOperators::instance(ops);
        dispatcher = DispatcherM68k::instance(ops, disassembler->wordSizeBytes()*8);
        dispatcher->currentState()->memoryState()->set_byteOrder(ByteOrder::ORDER_MSB);
    }
#endif

#if 0
    // PP (8/9/2022): metadata objects are loaded through the loader
    // DQ (11/7/2021): Start looking for the streams (Cil terminology) within the .text section.
    look_for_cil_streams(settings, map);
#endif

    SgAsmCilMetadataRoot* metadata = obtainMetaDataRoot();
    std::vector<uint32_t> rvaList = generateRVAs(metadata);

#if 0
    printf ("Exiting after unparsing the CIL streams (skipping disassembly of instructions until we can resolve RVA) \n");
    return 0;
#endif

    printf ("Generated rvaList.size() = %zu \n",rvaList.size());
    for (size_t i=0; i < rvaList.size(); i++)
       {
      // Note: the location of the instructions for each method are at the 0x00400000 (base of PE Header file) + RVA.
         uint32_t RVA = rvaList[i];
         printf ("rvaList[%zu] = RVA = %u 0x%x \n",i,RVA,RVA);
       }

    // Disassemble at each valid address, and show disassembly errors

    // rose_addr_t va = settings.startVa;
    // while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
    // Start at the first address of the RVA (what ever that is).

    rose_addr_t va = settings.startVa;

    printf ("Before reset using RVA: va = 0x%" PRIu64 " \n",va);

#if 1 /* XYZ */
 // Reset the va to incude the generated RVA
 // va = rvaList[0];
#if 1
    va = 0x00400000 + rvaList[0];
#endif

    printf ("After explicitly setting va(using 0x00400000 + RVA): va = 0x%" PRIu64 " \n",va);

 // DQ (12/20/2021): Robb sent me this line of code that is correct since our offset is into the map.
 // uint8_t method_header_leading_byte_value = ByteOrder::le_to_host(*((uint8_t*)va));
    uint8_t method_header_leading_byte_value = 0;
    size_t nRead = map->at(va).limit(1).read(&method_header_leading_byte_value).size();

    printf ("read n = %zu \n", nRead);
    ROSE_ASSERT(nRead == 1);

    printf ("method_header_leading_byte_value = 0x%x \n",method_header_leading_byte_value);

    // uint8_t method_header_leading_byte_value_alt_1 = ByteOrder::le_to_host(*((uint8_t*)rvaList[0]));
    // printf ("method_header_leading_byte_value_alt_1 = 0x%x \n",method_header_leading_byte_value_alt_1);

 // These are zero, could it be that we want the 2nd byte because of little-endian ordering?
    bool bit_1 = Rose::BitOps::bit(method_header_leading_byte_value,0);
    bool bit_2 = Rose::BitOps::bit(method_header_leading_byte_value,1);

    printf ("bit_1 = %s \n",bit_1 ? "true" : "false");
    printf ("bit_2 = %s \n",bit_2 ? "true" : "false");

#if 0
 // I get a different result if these are commented out?????
    bool bit_3 = Rose::BitOps::bit(method_header_leading_byte_value,2);
    bool bit_4 = Rose::BitOps::bit(method_header_leading_byte_value,3);
    bool bit_5 = Rose::BitOps::bit(method_header_leading_byte_value,4);
    bool bit_6 = Rose::BitOps::bit(method_header_leading_byte_value,5);
    bool bit_7 = Rose::BitOps::bit(method_header_leading_byte_value,6);
    bool bit_8 = Rose::BitOps::bit(method_header_leading_byte_value,7);

    printf ("bit_3 = %s \n",bit_3 ? "true" : "false");
    printf ("bit_4 = %s \n",bit_4 ? "true" : "false");
    printf ("bit_5 = %s \n",bit_5 ? "true" : "false");
    printf ("bit_6 = %s \n",bit_6 ? "true" : "false");
    printf ("bit_7 = %s \n",bit_7 ? "true" : "false");
    printf ("bit_8 = %s \n",bit_8 ? "true" : "false");
#endif

    if (bit_2 == true)
       {
         if (bit_1 == false)
            {
           // This is a tiny header
            }
           else
            {
           // This is a fat header
            }
       }

    printf ("After reset using RVA: va = 0x%" PRIu64 "\n",va);
#endif /* XYZ */

#if 1
    printf ("Exiting before processing instructions \n");
    return 0;
#endif

 // DQ (12/17/2021): Robb is sending a new while loop.
 // while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va))
 // while (map->at(va).require(MemoryMap::EXECUTABLE).next())

 // DQ (12/18/2021): Use the original while loop.
 // while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va))
    while (map->at(va).require(MemoryMap::EXECUTABLE).next())
       {
     // We likely don't need this for .NET
     // va = alignUp(va, settings.alignment);

     // DQ (12/17/2021): Need to accumulate the instructions into a list.
     // And attach the list to the MethodDef table (or that is an intermediate solution).
        try {
#if 0
            printf ("Before calling disassembler->disassembleOne(map, va): va = 0x%x \n",va);
#endif
#if 0
            printf ("Increment the va using the RVA for the first method \n");
            va = va + rvaList[0];
            printf ("Before calling disassembler->disassembleOne(map, va): va = 0x%x \n",va);
#endif
#if 0
            printf ("Reset the va using the RVA for the first method \n");
            va = rvaList[0];
            printf ("Before calling disassembler->disassembleOne(map, va): va = 0x%x \n",va);
#endif
            SgAsmInstruction *insn = disassembler->disassembleOne(map, va);
            ASSERT_not_null(insn);
            unparser.unparse(std::cout, insn);

            if (settings.runSemantics) {
                if (isSgAsmM68kInstruction(insn)) {
                    bool skipThisInstruction = false;
#if 0 // [Robb P. Matzke 2014-07-29]
                    switch (isSgAsmM68kInstruction(insn)->get_kind()) {
                        case m68k_cpusha:
                        case m68k_cpushl:
                        case m68k_cpushp:
                            std::cout <<"    No semantics yet for privileged instructions\n";
                            skipThisInstruction = true;
                            break;

                        case m68k_fbeq:
                        case m68k_fbne:
                        case m68k_fboge:
                        case m68k_fbogt:
                        case m68k_fbule:
                        case m68k_fbult:
                        case m68k_fcmp:
                        case m68k_fdabs:
                        case m68k_fdadd:
                        case m68k_fddiv:
                        case m68k_fdiv:
                        case m68k_fdmove:
                        case m68k_fdmul:
                        case m68k_fdneg:
                        case m68k_fdsqrt:
                        case m68k_fdsub:
                        case m68k_fintrz:
                        case m68k_fmove:
                        case m68k_fmovem:
                        case m68k_fsadd:
                        case m68k_fsdiv:
                        case m68k_fsmove:
                        case m68k_fsmul:
                        case m68k_fsneg:
                        case m68k_fssub:
                        case m68k_ftst:
                            std::cout <<"    No semantics yet for floating-point instructions\n";
                            skipThisInstruction = true;
                            break;

                        case m68k_nbcd:
                        case m68k_rtm:
                        case m68k_movep:
                            std::cout <<"    No semantics yet for this odd instruction\n";
                            skipThisInstruction = true;
                            break;

                        default:
                            break;
                    }
#endif

                    if (!skipThisInstruction) {
                        //ops->currentState()->clear();
                        dispatcher->processInstruction(insn);
                        std::ostringstream ss;
                        ss <<*dispatcher->currentState();
                        std::cout <<StringUtility::prefixLines(ss.str(), "    ") <<"\n";
                    }
                }
            }


            va += insn->get_size();
            if (0 != va % settings.alignment)
                std::cerr <<StringUtility::addrToString(va) <<": invalid alignment\n";
#if 0 // [Robb P. Matzke 2014-06-19]: broken
            deleteAST(insn);
#endif
        } catch (const Disassembler::Exception &e) {
            std::cerr <<StringUtility::addrToString(va) <<": " <<e.what() <<"\n";
            ++va;
        }
    }

#if 1
    printf ("Leaving main() \n");
#endif

    exit(0);
}
