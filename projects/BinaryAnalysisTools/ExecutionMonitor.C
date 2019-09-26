
static const char* purpose = "Monitors the execution of some program and computes some coverage number.";

static const char* description =
    "Monitors the execution of a program and produces some coverage number. "
    "The coverage number is an estimate how many unique instructions were executed. "
    "To this end, execmon traces the execution and records code "
    "intervals. Code intervals are non-overlapping, contiguous code segments.\n";

#include <algorithm>
#include <fstream>

#include "sage3basic.h"
#include "Sawyer/IntervalSet.h"
#include "Sawyer/Interval.h"
#include "BinaryDebugger.h"
#include "Disassembler.h"
#include "CommandLine.h"
#include "Diagnostics.h"
#include "Partitioner2/Engine.h"



// Architecture specific defines

#if (  defined(_ARCH_PPC) || defined(_ARCH_PPC64) /* XLC + GNU */ \
    || defined(__mips__) || defined(__mips)       /* GNU */       \
    )

  static const size_t INSTRUCTION_LENGTH = 4; /* bytes */

  /*
   * - PowerPC: All PowerPCs (including 64-bit implementations) use fixed-length 32-bit instructions.
   *            https://www.ibm.com/developerworks/library/l-ppc/index.html
   * - MIPS:    All instructions are 32 bit
   *            http://people.cs.pitt.edu/~xujie/cs447/MIPS_Instruction.htm
   */

#else
  static const size_t INSTRUCTION_LENGTH = 0; /* variable-length encoding */

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


//
// anonymous namespace for auxiliary functions and functors

namespace
{
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

//
// core functionality

namespace Rose {
namespace BinaryAnalysis {

  struct ExecutionMonitor : Debugger
  {
      /* this is an imprecise estimate. */
      static const size_t INSTR_LENGTH_ESTIMATE = 4;
      static const size_t INSTR_LENGTH_MARGIN   = 1;

      typedef rose_addr_t                                     addr_t;
      typedef Sawyer::Container::Interval<addr_t>             AddressInterval;
      typedef Sawyer::Container::IntervalSet<AddressInterval> AddressIntervalSet;

/*
      ExecutionMonitor(const boost::filesystem::path& exeName, const std::vector<std::string>& args, Disassembler* disasm) 
      : Debugger(exeName, args, Debugger::CLOSE_FILES),
        disassembler(disasm), currIval(), intervals()
      {}
*/
      ExecutionMonitor(const std::vector<std::string>& exeNameAndArgs, Disassembler* disasm)
          : disassembler(disasm), currIval(), intervals()
      {
          Debugger::Specimen specimen(exeNameAndArgs);
          specimen.flags().set(Debugger::CLOSE_FILES);
          attach(specimen);
      }

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

      /** Tests whether @ref instrAddr is right-adjacent to the interval @ref curr. */
      bool adjacentInstruction(const AddressInterval& curr, addr_t instrAddr) const
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
      Disassembler* const disassembler; ///< disassembler for architectures
                                        ///<   using variable-length instruction encoding.
      AddressInterval     currIval;     ///< current code interval
      AddressIntervalSet  intervals;    ///< all code intervals

      //
      // ExecutionMonitor() = delete;
  };

  void ExecutionMonitor::step()
  {
    const addr_t addr = pc();

    // test whether the PC is right-adjacent to the current code segment
    if (!adjacentInstruction(currIval, addr))
    {
      // record previous execution range, and start a new one
      intervals.insert(currIval);
      currIval = AddressInterval(addr);
    }

    // extend address interval
    currIval = currIval.hull(addr + instructionLength(addr));

    // make a move
    singleStep();
  }

  void ExecutionMonitor::run()
  {
    currIval = AddressInterval(pc());

    while (!isTerminated())
      step();

    intervals.insert(currIval);
  }
}
}


//
// command line and option handling

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

  out.insert( Switch("output", 'o')
              .argument("file", anyParser(settings.outputFileName))
              .doc("Write the result to the specified file.")
            );

  SwitchGroup decoding("Decoding switches");

  decoding.insert( Switch("no-disassembler")
                   .intrinsicValue("false", booleanParser(settings.disassembler))
                   .doc( "Use some instruction length estimate on systems "
                         "with variable-length instruction encoding, instead "
                         "of a more precise but computed instruction length. "
                         "Trades precision for speed.\n"
                         "Ignored on systems with fixed-length instructions (e.g., PowerPC).\n"
                       )
                  );
  Parser parser;
  parser
      .purpose(purpose)
      .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
      .chapter(1, "ROSE Command-line Tools")
      .doc("Synopsis", "@prop{programName} [@v{switches}] -- @v{specimen} [@v{args}...]")
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

  char** const            sep = std::find_if(argv, argv+argc, IsSeparator());
  char**                  specpos = sep;
  if (specpos != argv+argc) ++specpos;

  std::vector<std::string> specimenAndArgs(specpos, argv+argc);
  Disassembler*            disassembler = NULL;

  // Parse command-line
  P2::Engine               engine;
  Settings                 settings;
  std::vector<std::string> unused = parseCommandLine(sep-argv, argv, engine, settings);

  for (size_t i = 0; i < unused.size(); ++i)
  {
    ::mlog[WARN] << "unknown argument: " << unused.at(i) << std::endl;
  }
  
  if (specimenAndArgs.size() == 0)
  {
    ::mlog[FATAL] << "No specimen given (use --help)\n";
    exit(1);
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
    if (!engine.parseContainers(specimenAndArgs.front()))
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

  ExecutionMonitor execmon(specimenAndArgs, disassembler);

  execmon.run();

  traceOutput << execmon.waitpidStatus() << '\n'
              << execmon.estimateDistinctInstructionBytes()
              << std::endl;

  return 0;
}
