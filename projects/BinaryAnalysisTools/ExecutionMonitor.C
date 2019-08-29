
static const char *purpose = "Estimate the number of distinct instructions that are executed";
static const char *description =
    "Runs the specimen in a debugger and tracks instructions addresses.";


// NAME
//   execmon - monitors the execution of some program and
//             computes some coverage number.
//
// SYNOPSIS
//   execmon [arguments] -- program [program-arguments]
//
// DESCRIPTION
//   Monitors the execution of a program and produces some coverage number.
//   The coverage number is an estimate how many unique instructions were
//   executed.
//   To this end, execmon traces the execution and records code
//   intervals. Code intervals are non-overlapping, and in some runs
//   contiguously executed code segments.
//
//   Coverage number := sum_{i=0}^{#segments}(instrEstimate(segment_i))
//
//   As opposed to some RISC architectures, other systems (e.g., x86
//   and derivatives) use a variable instruction length. To compute the
//   instrEstimate, execmon assumes an average length (maybe just
//   bytecount would suffice).
//
//   --perf=intel  uses perf on intel systems /* TODO */

#include <algorithm>

#include "sage3basic.h"
#include "Sawyer/IntervalSet.h"
#include "Sawyer/Interval.h"
#include "BinaryDebugger.h"
#include "Disassembler.h"
#include "CommandLine.h"
#include "Diagnostics.h"
#include "Partitioner2/Engine.h"



// Architecture specific defines:
//   https://sourceforge.net/p/predef/wiki/Architectures/

#if (  defined(_ARCH_PPC) || defined(_ARCH_PPC64) /* XLC + GNU */ \
    || defined(__mips__) || defined(__mips)       /* GNU */       \
    )

  #define INSTRUCTION_LENGTH 4 /* bytes */

  /*
   * - PowerPC: All PowerPCs (including 64-bit implementations) use fixed-length 32-bit instructions.
   *            https://www.ibm.com/developerworks/library/l-ppc/index.html
   * - MPIS:    All instructions are 32 bit
   *            http://people.cs.pitt.edu/~xujie/cs447/MIPS_Instruction.htm
   */

#else
  #define INSTRUCTION_LENGTH 0 /* variable-length encoding */

  /* variable-length instruction encoding requires disassembler
   *
   * - x86: up to 15 bytes
   *        https://wiki.osdev.org/X86-64_Instruction_Encoding
   *
   * \todo
   * - ARM:     ??
   * - other:   ??
   */
#endif /* instruction length */


namespace P2 = Rose::BinaryAnalysis::Partitioner2;

using namespace Sawyer::Message::Common;


//
// globals

Sawyer::Message::Facility mlog;

namespace
{
  // anonymous namespace for auxiliary functions and functors

  struct IsSeparator
  {
    bool operator()(const char* str)
    {
      return std::equal(separator, separator + sizeof(separator)/sizeof(char), str);
    }

    static const char separator[3];
  };

  const char IsSeparator::separator[3] = { '-', '-', 0 };

  // "borrowed" from nativeExecutionTrace
  SgAsmInstruction*
  disassembleOne(Rose::BinaryAnalysis::Disassembler* disasm, const uint8_t* buf, size_t bufSize, rose_addr_t ip)
  {
    try
    {
      return disasm->disassembleOne(buf, ip, bufSize, ip);
    }
    catch (const Rose::BinaryAnalysis::Disassembler::Exception &e)
    {
      mlog[WARN] << "cannot disassemble instruction at "
                 << reinterpret_cast<void*>(ip) << ": " << e.what()
                 << "\n";
    }

    return NULL;
  }
}



namespace Rose {
namespace BinaryAnalysis {

  struct ExecutionMonitor : BinaryDebugger
  {
      /* this is preliminary and does not hold on x86 */
      static constexpr size_t INSTR_LENGTH_ESTIMATE = 4;
      static constexpr size_t INSTR_LENGTH_MARGIN   = 1;

      typedef rose_addr_t                                     addr_t;
      typedef Sawyer::Container::Interval<addr_t>             AddressInterval;
      typedef Sawyer::Container::IntervalSet<AddressInterval> AddressIntervalSet;

      ExecutionMonitor(const std::vector<std::string>& exeNameAndArgs, Disassembler* disasm)
      : BinaryDebugger(exeNameAndArgs, BinaryDebugger::CLOSE_FILES),
        disassembler(disasm), currIval(), intervals()
      {}

      /** Returns the current program counter (PC). */
      addr_t pc()
      {
        return executionAddress();
      }

      /** Returns the length of the current instruction in bytes. */
      size_t instructionLength(addr_t ip)
      {
        typedef std::auto_ptr<SgAsmInstruction> SageAsmInstruction;

        if (INSTRUCTION_LENGTH)
          return INSTRUCTION_LENGTH;

        size_t instrlen = INSTR_LENGTH_ESTIMATE;

        if (disassembler)
        {
          // borrowed from nativeExecutionTrace
          uint8_t            buf[16]; // 16 should be large enough for any instruction
          size_t             nBytes = readMemory(ip, sizeof buf, buf);
          SageAsmInstruction insn(disassembleOne(disassembler, buf, nBytes, ip));

          if (insn.get()) instrlen = insn->get_size();
        }

        return instrlen;
      }

      /** Tests whether pc() is adjacent to the interval @ref ival. */
      bool adjacentInstruction(const AddressInterval& curr, addr_t instrAddr)
      {
        // if instrAddr is before curr, we start a new interval
        // if instrAddr is inside curr, we start a new interval (could continue in current one)
        if (curr.greatest() > instrAddr) return false;

        // test if instrAddr is within the interval's right proximity
        return (instrAddr - curr.greatest()) <= INSTR_LENGTH_MARGIN;
      }

      /** Executes a single instruction and manages the
       *  instruction range interval. */
      void step();

      /** Runs the program to termination. */
      void run();

      /** Returns an estimated number of unique instructions executed
       *  during the program run. */
      size_t estimateDistinctInstructionBytes() const
      {
        return intervals.size();
      }

    private:
      Disassembler*      disassembler;
      AddressInterval    currIval;  //< current interval
      AddressIntervalSet intervals;

      //
      // ExecutionMonitor() = delete;
  };

  void ExecutionMonitor::step()
  {
    const addr_t addr = pc();

    if (adjacentInstruction(currIval, addr))
    {
      // expand current interval
      currIval = currIval.hull(addr + instructionLength(addr));
    }
    else
    {
      // record previous execution range;
      intervals.insert(currIval);

      // start new execution range
      currIval = AddressInterval(addr);
    }

    // make a move
    singleStep();
  }

  void ExecutionMonitor::run()
  {
    currIval = AddressInterval(pc());

    while (!isTerminated())
      step();
  }
}
}


struct Settings
{
  Settings()
  : outputFileName(), disassembler(true)
  {}

  boost::filesystem::path outputFileName;
  bool                    disassembler;
};


static
std::vector<std::string>
parseCommandLine(int argc, char** argv, P2::Engine& engine, Settings& settings)
{
  using namespace Sawyer::CommandLine;

  SwitchGroup out("Output switches");

  out.insert(Switch("output", 'o')
             .argument("file", anyParser(settings.outputFileName))
             .doc("Write the result to the specified file."));

  SwitchGroup decoding("Decoding switches");

  decoding.insert(Switch("no-disassembler")
                  .intrinsicValue("false", booleanParser(settings.disassembler))
                  .doc("disable disassembler on architectures using variable-length instructions."));

  Parser parser;
  parser
      .purpose(purpose)
      .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
      .chapter(1, "ROSE Command-line Tools")
      .doc("Synopsis", "@prop{programName} [@v{switches}] @v{specimen} [@v{args}...]")
      .doc("Description", description)
      .with(engine.engineSwitches())
      .with(out)
      .with(decoding);

  return parser.parse(argc, argv).apply().unreachedArgs();
}



int main(int argc, char** argv)
{
  using Rose::BinaryAnalysis::ExecutionMonitor;
  using Rose::BinaryAnalysis::Disassembler;

  ROSE_INITIALIZE;
  Rose::Diagnostics::initAndRegister(&::mlog, "tool");

  char**                   sep = std::find_if(argv, argv+argc, IsSeparator());
  std::vector<std::string> specimen(sep + 1, argv+argc);
  Disassembler*            disassembler = NULL;

  // Parse command-line
  P2::Engine               engine;
  Settings                 settings;
  std::vector<std::string> unused = parseCommandLine(sep-argv, argv, engine, settings);

  for (size_t i = 0; i < unused.size(); ++i)
  {
    ::mlog[FATAL] << "argument ignored: " << unused.at(i) << std::endl;
  }

  // Trace output goes to either std::cout or some file.
  std::filebuf             fb;
  const bool               useFile = !settings.outputFileName.empty();

  if (useFile)
  {
    fb.open(settings.outputFileName.native().c_str(), std::ios::out);
  }

  std::ostream             traceOutput(useFile ? &fb : std::cout.rdbuf());

  if (INSTRUCTION_LENGTH == 0 && settings.disassembler)
  {
    // Load specimen into ROSE's simulated memory
    if (!engine.parseContainers(specimen.front()))
    {
      ::mlog[FATAL] << "cannot parse specimen binary container\n";
      exit(1);
    }

    disassembler = engine.obtainDisassembler();

    if (!disassembler)
    {
      ::mlog[FATAL] << "no disassembler for this architecture\n";
      exit(1);
    }
  }

  ExecutionMonitor execmon(specimen, disassembler);

  execmon.run();
  traceOutput << "#estimated unique instruction bytes = " << execmon.estimateDistinctInstructionBytes() << std::endl;

  return 0;
}
