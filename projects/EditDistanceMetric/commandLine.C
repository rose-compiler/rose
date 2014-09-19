#include <rose.h>
#include "commandLine.h"

// Parses only those switches that this tool recognizes and skips all others, leaving them for ROSE's frontend(). The tool
// switches come first and the ROSE switches are after a "--" separator switch.  We wouldn't need to do it this way, but three
// things conspire to make this the best approach: (1) frontend() doesn't throw an error when it doesn't recognize a switch,
// (2) ROSE doesn't provide a description of its command-line, and (3) ROSE doesn't provide a mechanism to check the validity
// of a command line without actually applying it.
Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    // FIXME[Robb P. Matzke 2014-09-18]: These generic switches should be provided by a library function.
    SwitchGroup gen("General switches.");
    gen.insert(Switch("help", 'h')
               .doc("Show this documentation.")
               .action(showHelpAndExit(0)));
    gen.insert(Switch("log", 'L')
               .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
               .argument("config")
               .whichValue(SAVE_ALL)
               .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));
    gen.insert(Switch("version", 'V')
               .action(showVersionAndExit(version_message(), 0))
               .doc("Shows version information for various ROSE components and then exits."));

    SwitchGroup tool("Switches for this tool.");
    tool.insert(Switch("insertion-cost")
                .argument("cost", realNumberParser(settings.insertionCost))
                .doc("Non-negative cost for performing an insertion edit. The default is " +
                     boost::lexical_cast<std::string>(settings.insertionCost) + "."));
    tool.insert(Switch("deletion-cost")
                .argument("cost", realNumberParser(settings.deletionCost))
                .doc("Non-negative cost for performing a deletion edit. The default is " +
                     boost::lexical_cast<std::string>(settings.deletionCost) + "."));
    tool.insert(Switch("substitution-cost")
                .argument("cost", realNumberParser(settings.substitutionCost))
                .doc("Non-negative cost for performing a substitution edit. The default is " +
                     boost::lexical_cast<std::string>(settings.substitutionCost) + "."));

    Parser parser;
    parser
        .purpose("demonstrates tree edit distance")
        .doc("synopsis",
             "@prop{programName} [@v{tool_switches}] -- [@v{rose_switches}] @v{specimen1} @v{specimen2}")
        .doc("description",
             "This tool performs tree edit distance between two source files a couple different ways. It uses the "
             "original local implementation that returned no useful information and printed its results as a side effect, "
             "and it uses the TreeEditDistance analysis that is now part of librose."
             "\n\n"
             "Note that the edit costs specified on the command line are only used for the librose version of tree edit "
             "distance; the local implementation always uses 1.0 for all costs.");

    return parser.with(gen).with(tool).parse(argc, argv).apply();
}

