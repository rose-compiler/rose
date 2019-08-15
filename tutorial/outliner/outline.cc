// The "purpose" as it appears in the man page, uncapitalized and a single, short, line.
static const char *purpose = "This tool outlines code segments to functions";

static const char *description =
    "Outlining is the process of replacing a block of consecutive "
    "statements with a function call to a new function containing those statements. "
    "Conceptually, outlining is the inverse of inlining.";

#include <rose.h>                                       // must be first ROSE include
#include <Outliner.hh>                                  // from ROSE
#include <CommandLine.h>                                // from ROSE
#include <Diagnostics.h>                                // from ROSE

#include <iostream>
#include <vector>
#include <string>

using namespace Rose;                                   // the ROSE team is migrating everything to this namespace
using namespace Rose::Diagnostics;                      // for mlog, INFO, WARN, ERROR, etc.

Diagnostics::Facility mlog;                             // most ROSE tools use a diagnostics logging facility

// Switches for this tool. Tools with lots of switches will probably want these to be in some Settings struct mirroring the
// approach used by some analyses that have lots of settings. So we'll do that here too even though it looks funny.
struct Settings {
    bool showOutlinerSettings;                          // should we show the outliner settings instead of running it?
    bool useOldParser;                                  // call the old Outliner command-line parser

    Settings()
        : showOutlinerSettings(false), useOldParser(false) {}
} settings;

// Parse the tool's command-line, processing only those switches recognized by Sawyer. Then return the non-parsed switches for
// the next stage of parsing. We have three more stages that need to process the command-line: Outliner (the old approach),
// frontend(), and the backend compiler. None of these except the backend compiler can issue error messages about misspelled
// switches because the first three must assume that an unrecognized switch is intended for a later stage.
static std::vector<std::string>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    // Use Rose::CommandLine to create a consistent parser among all tools.  If you want a tool's parser to be different
    // then either create one yourself, or modify the parser properties after createParser returns. The createEmptyParserStage
    // creates a parser that assumes all unrecognized switches are intended for a later stage. If there are no later stages
    // then use createEmptyParser instead or else users will never see error messages for misspelled switches.
    Parser p = Rose::CommandLine::createEmptyParserStage(purpose, description);
    p.doc("Synopsis", "@prop{programName} @v{switches} @v{files}...");
#if 1 // DEBUGGING [Robb P Matzke 2016-09-27]
    p.longPrefix("-");
#endif

    // User errors (what few will be reported since this is only a first-stage parser) should be sent to standard error instead
    // of raising an exception.  Programmer errors still cause exceptions.
    p.errorStream(::mlog[FATAL]);

    // All ROSE tools have some switches in common, such as --version, -V, --help, -h, -?, --log, -L, --threads, etc. We
    // include them first so they appear near the top of the documentation.  The genericSwitches returns a
    // Sawyer::CommandLine::SwitchGroup, which this tool could extend by adding additional switches.  This could have been done
    // inside createParser, but it turns out that many tools like to extend or re-order this group of switches, which is
    // simpler this way.
    p.with(Rose::CommandLine::genericSwitches());

    // Eventually, if we change frontend so we can query what switches it knows about, we could insert them into our parser at
    // this point.  The frontend could report all known switches (sort of how things are organized one) or we could query only
    // those groups of frontend switches that this tool is interested in (e.g., I don't know if the outliner needs Fortran
    // switches).
#if 0 // [Robb P Matzke 2016-09-27]
    p.with(CommandlineProcessing::frontendAllSwitches()); // or similar
#endif

    // The Outliner has some switches of its own, so include them next.  These switches will automatically adjust the Outliner
    // settings. Since the outliner is implemented as a namespace rather than a class, it's essentially a singlton.  There can
    // be only one instance of an outliner per tool, whether the tool uses an outliner directly (like this one) or indirectly
    // as part of some other analysis.
    p.with(Outliner::commandLineSwitches());

    // Finally, a tool sometimes has its own specific settings, so we demo that here with a couple made-up switches.
    SwitchGroup tool("Tool-specific switches");

    tool.insert(Switch("dry-run", 'n')
                .intrinsicValue(true, settings.showOutlinerSettings)
                .doc("Instead of running the outliner, just display its settings."));

    // Helper function that adds "--old-outliner" and "--no-old-outliner" to the tool switch group, and causes
    // settings.useOldParser to be set to true or false. It also appends some additional documentation to say what the default
    // value is. We could have done this by hand with Sawyer, but having a helper encourages consistency.
    Rose::CommandLine::insertBooleanSwitch(tool, "old-outliner", settings.useOldParser, 
                                           "Call the old Outliner parser in addition to its new Sawyer parser.");

    // We want the "--rose:help" switch to appear in the Sawyer documentation but we have to pass it to the next stage also. We
    // could do this two different ways. The older way (that still works) is to have Sawyer process the switch and then we
    // prepend it into the returned vector for processing by later stages.  The newer way is to set the switch's "skipping"
    // property that causes Sawyer to treat it as a skipped (unrecognized) switch.  We'll use SKIP_STRONG, but SKIP_WEAK is
    // sort of a cross between Sawyer recognizing it and not recognizing it.
    tool.insert(Switch("rose:help")
                .skipping(SKIP_STRONG)                  // appears in documentation and is parsed, but treated as skipped
                .doc("Show the non-Sawyer switch documentation."));

    // Copy this tool's switches into the parser.
    p.with(tool);

    // Parse the command-line, stopping at the first "--" or positional arugment. Return the unparsed stuff so it can be passed
    // to the next stage.  ROSE's frontend expects arg[0] to be the name of the command, which Sawyer has already processed, so
    // we need to add it back again.
    std::vector<std::string> remainingArgs = p.parse(argc, argv).apply().unparsedArgs(true);
    remainingArgs.insert(remainingArgs.begin(), argv[0]);

#if 1 // DEBUGGING [Robb P Matzke 2016-09-27]
    std::cerr <<"These are the arguments after parsing with Sawyer:\n";
    BOOST_FOREACH (const std::string &s, remainingArgs)
        std::cerr <<"    \"" <<s <<"\"\n";
#endif

    return remainingArgs;
}

int
main (int argc, char* argv[])
{
  // Initialize and check compatibility. See Rose::initialize
  ROSE_INITIALIZE;
  ::mlog = Diagnostics::Facility("tool", Diagnostics::destination);
  Diagnostics::mfacilities.insertAndAdjust(::mlog);

  // Parse Sawyer-recognized switches and the rest we'll pass to Outliner and frontend like before.
  std::vector<std::string> args = parseCommandLine(argc, argv);
  if (settings.useOldParser)
      Outliner::commandLineProcessing(args);  // this is the old way
  SgProject *proj = frontend(args);

  // Binary analyses know to validate their user-defined settings before they do any analysis, but that's not true (yet) for
  // Outliner, so I've created a function we can call explicitly.
  Outliner::validateSettings();

  if (settings.showOutlinerSettings) {
      std::cout <<"Outliner settings:\n";
      std::cout <<"  enable_debug        = " <<Outliner::enable_debug <<"\n";
      std::cout <<"  preproc_only_       = " <<Outliner::preproc_only_ <<"\n";
      std::cout <<"  useParameterWrapper = " <<Outliner::useParameterWrapper <<"\n";
      std::cout <<"  useStructureWrapper = " <<Outliner::useStructureWrapper <<"\n";
      std::cout <<"  useNewFile          = " <<Outliner::useNewFile <<"\n";
      std::cout <<"  exclude_headers     = " <<Outliner::exclude_headers <<"\n";
      std::cout <<"  enable_classic      = " <<Outliner::enable_classic <<"\n";
      std::cout <<"  temp_variable       = " <<Outliner::temp_variable <<"\n";
      std::cout <<"  use_dlopen          = " <<Outliner::use_dlopen <<"\n";
      std::cout <<"  handles             = [";
      BOOST_FOREACH (const std::string &handle, Outliner::handles)
          std::cout <<" " <<handle;
      std::cout <<" ]\n";
      std::cout <<"  output_path         = " <<Outliner::output_path <<"\n";
      std::cout <<"  enable_liveness     = " <<Outliner::enable_liveness <<"\n";
  } else {
      // Ideally, this logging would be part of the Outliner, not every tool that uses it. And the outliner would have its own
      // diagnostic facility that could be turned on and off from the command-line (see --log).
      ::mlog[INFO] << "outlining...\n";
      size_t count = Outliner::outlineAll(proj);
      ::mlog[INFO] << "outlining processed " << StringUtility::plural(count, "outline directives") <<"\n";
      return backend(proj);
  }
  return 0;
}
