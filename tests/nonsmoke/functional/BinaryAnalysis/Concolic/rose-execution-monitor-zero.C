
static const char* purpose = "Monitors the execution of some program and computes some coverage number.";

static const char* description =
    "Monitors the execution of a program and produces some coverage number. "
    "The coverage number is an estimate how many unique instructions were executed. "
    "To this end, execmon traces the execution and records code "
    "intervals. Code intervals are non-overlapping, contiguous code segments.\n";

#include <algorithm>
#include <set>
#include <fstream>

#include "sage3basic.h"
#include "BinaryDebugger.h"
#include "CommandLine.h"
#include "Diagnostics.h"
#include "Partitioner2/Engine.h"

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

using namespace Sawyer::Message::Common;


//
// globals

Sawyer::Message::Facility mlog;

//
// core functionality

namespace Rose {
namespace BinaryAnalysis {

  struct ExecutionMonitor : Debugger
  {
      typedef rose_addr_t                  addr_t;
      typedef std::set<addr_t>             AddressSet;

      ExecutionMonitor(const std::vector<std::string>& exeNameAndArgs)
          : addresses()
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

      
      /** Executes a single instruction and manages the
       *  instruction range interval. */
      void step();

      /** Runs the program to termination. */
      void run();

      /** Returns an estimated number of unique instructions executed
       *  during the program run. */
      size_t estimateDistinctInstructionBytes() const
      {
        return addresses.size();
      }

    private:
      AddressSet addresses;    ///< all code intervals

      //
      // ExecutionMonitor() = delete;
  };

  void ExecutionMonitor::step()
  {
    addresses.insert(pc());

    // make a move
    singleStep();
  }

  void ExecutionMonitor::run()
  {
    while (!isTerminated())
      step();
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

  // Parse command-line
  P2::Engine               engine;
  Settings                 settings;
  std::vector<std::string> specimenAndArgs = parseCommandLine(argc, argv, engine, settings);
  
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

  ExecutionMonitor         execmon(specimenAndArgs);

  execmon.run();

  traceOutput << execmon.waitpidStatus() << '\n'
              << execmon.estimateDistinctInstructionBytes()
              << std::endl;

  return 0;
}
