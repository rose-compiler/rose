// The "purpose" as it appears in the man page, uncapitalized and a single, short, line.
static const char *purpose = "demonstrates the pragma-interface of the outliner";

static const char *description =
    "This is the full, multi-paragraph description of this tool with all relevant details. It needs to be written yet.";


#include <rose.h>                                       // must be first ROSE include
#include <Outliner.hh>                                  // from ROSE
#include <Diagnostics.h>                                // from ROSE

#include <iostream>
#include <vector>
#include <string>

using namespace rose;                                   // the ROSE team is migrating everything to this namespace
using namespace rose::Diagnostics;                      // for mlog, INFO, WARN, ERROR, etc.

Diagnostics::Facility mlog;                             // most ROSE tools use a diagnostics logging facility

// Switches for this tool. Tools with lots of switches will probably want these to be in some Settings struct mirroring the
// approach used by some analyses that have lots of settings. So we'll do that here too even though it looks funny.
struct Settings {
    bool showOutlinerSettings;                          // should we show the outliner settings instead of running it?
    bool useOldParser;                                  // call the old Outliner command-line parser

    Settings()
        : showOutlinerSettings(false), useOldParser(false) {}
} settings;

// Parse the tool's command-line. We want to use a Sawyer::CommandLine::Parser for various reasons, but this is currently a bit
// of a kludge because most of ROSE doesn't yet support Sawyer. Therefore, since we have two command-line parsers (one for this
// tool and the outliner analysis, and another for the rest of ROSE), we'll require that the Sawyer switches appear before the
// non-Sawyer switches and if there are non-sawyer switches they must be introduced with a special "--" separator argument.
//
// Other approaches are possible, such as configuring the Sawyer parser to skip over anything it doesn't understand, but those
// approaches all have problems with mis-parsing due to incomplete information in whichever parser is invoked first.
static SgProject*
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    // Use CommandlineProcessing to create a consistent parser among all tools.  If you want a tool's parser to be different
    // then either create one yourself, or modify the parser properties after createParser returns.
    Parser p = CommandlineProcessing::createEmptyParser(purpose, description);
    p.errorStream(::mlog[FATAL]);                       // on error, show message and exit rather than throw exception
    p.doc("Synopsis", "@prop{programName} [@v{outliner_switches}] [-- @v{rose_switches}] @v{files}");

    // All ROSE tools have some switches in common, such as --version, -V, --help, -h, -?, --log, -L, --threads, etc. We
    // include them first so they appear near the top of the documentation.  The genericSwitches returns a
    // Sawyer::CommandLine::SwitchGroup, which this tool could extend by adding additional switches.  This could have been done
    // inside createParser, but it turns out that many tools like to extend or re-order this group of switches, which is
    // simpler this way.
    p.with(CommandlineProcessing::genericSwitches());

    // The Outliner has some switches of its own, so include them next.  These switches will automatically adjust the Outliner
    // settings. Since the outliner is implemented as a namespace rather than a class, it's essentially a singlton.  There can
    // be only one instance of an outliner per tool, whether the tool uses an outliner directly (like this one) or indirectly
    // as part of some other analysis.
    p.with(Outliner::commandLineSwitches());

    // Finally, a tool sometimes has its own specific settings, so we demo that here.  We're also adding a "--rose:help" switch
    // because Sawyer's parser only knows about switches declared with Sawyer and thus can only generate a manpage that
    // contains those switches.  If we see "--rose:help" we'll add "--help" to the frontend() arguments (and just to change
    // things up a bit, we'll use a local variable for this instead of the "settings" struct).
    SwitchGroup tool("Tool-specific switches");

    tool.insert(Switch("dry-run", 'n')
                .intrinsicValue(true, settings.showOutlinerSettings)
                .doc("Instead of running the outliner, just display its settings."));

    tool.insert(Switch("old-parser")
                .intrinsicValue(true, settings.useOldParser)
                .doc("Call the old command-line parser. This is in addition to the Sawyer parser."));

    bool showRoseHelp = false;
    tool.insert(Switch("rose:help")
                .intrinsicValue(true, showRoseHelp)
                .doc("Show the non-Sawyer switch documentation."));

    p.with(tool);                                       // the "tool" group is copied into the parser

    // Parse the command-line, stopping at the first "--" or positional arugment.
    std::vector<std::string> frontendArgs = p.parse(argc, argv).apply().unreachedArgs();

    // Pass the rest of the command-line to the other parser, frontend, which expects argv[0] at the beginning.
    if (showRoseHelp)
        frontendArgs.insert(frontendArgs.begin(), "--help");
    frontendArgs.insert(frontendArgs.begin(), argv[0]);
    if (settings.useOldParser)
        Outliner::commandLineProcessing(frontendArgs);  // this is the old way
    SgProject *project = frontend(frontendArgs);

    // Binary analyses know to validate their user-defined settings before they do any analysis, but that's not true (yet) for
    // Outliner, so I've created a function we can call explicitly.
    Outliner::validateSettings();
    return project;
}

int
main (int argc, char* argv[])
{
  // Initialize and check compatibility. See rose::initialize
  ROSE_INITIALIZE;
  ::mlog = Diagnostics::Facility("tool", Diagnostics::destination);
  Diagnostics::mfacilities.insertAndAdjust(::mlog);

  SgProject *proj = parseCommandLine(argc, argv);
  ASSERT_not_null(proj);

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
}
