#include <sage3basic.h>

#include <boost/foreach.hpp>
#include <sawyer/CommandLine.h>
#include <string>
#include <vector>

// The following symbols should be defined on our compile command line as string literals from variables of the same name in
// the ROSE makefiles (shown here with example values):
//  #define CXX "gcc"
//  #define CPPFLAGS "-I/home/user/boost/include"
//  #define CFLAGS "-O3 -fomit-frame-pointer -pthread"
//  #define LDFLAGS "-L/home/user/boost/lib -lboost_regex -pthread"

// Parse switches and return non-switch arguments
static std::vector<std::string>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    Parser parser;
    parser
        .purpose("show ROSE compilation arguments")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{command}")
        .doc("Description",
             "This command displays various switches that are useful in user makefiles when compiling or linking a program "
             "that uses ROSE.  The @v{command} can be any of the following words:"
             "@named{cxx}{Displays the name of the C++ compiler used to compile the ROSE library.}"
             "@named{cppflags}{Shows the switches that should be passed to the C preprocessor (and compile commands).}"
             "@named{cflags}{Shows the switches that are necessary when compiling a program that includes ROSE header files.}"
             "@named{ldflags}{Shows the switches that are necessary when linking a program that uses the ROSE library.}");

    SwitchGroup switches;
    switches.insert(Switch("help", 'h')
                    .doc("Show this documentation.")
                    .action(showHelpAndExit(0)));

    switches.insert(Switch("version", 'V')
                    .action(showVersionAndExit(version_message(), 0))
                    .doc("Shows version information for various ROSE components and then exits."));

    return parser.with(switches).parse(argc, argv).apply().unreachedArgs();
}

// Escape special characters for including in a makefile string.
// FIXME[Robb P. Matzke 2015-04-14]: for now, just use C-style escaping, which might be good enough.
static std::string
makefileEscape(const std::string &s) {
    return StringUtility::cEscape(s);
}

int
main(int argc, char *argv[]) {

#if 1 // DEBUGGING [Robb P. Matzke 2015-04-14]
    throw std::runtime_error("This command is not ready for end users yet; output may be incorrect!");
#endif

    std::vector<std::string> commands = parseCommandLine(argc, argv);
    if (commands.empty())
        throw std::runtime_error("no command specified; see --help");
    BOOST_FOREACH (const std::string &command, commands) {
        if (command == "cxx") {
            std::cout <<makefileEscape(CXX) <<"\n";
        } else if (command == "cppflags") {
            std::cout <<makefileEscape(CPPFLAGS) <<"\n";
        } else if (command == "cflags") {
            std::cout <<makefileEscape(CFLAGS) <<"\n";
        } else if (command == "ldflags") {
            std::cout <<makefileEscape(LDFLAGS) <<"\n";
        } else {
            throw std::runtime_error("unrecognized command: \"" + StringUtility::cEscape(command) + "\"");
        }
    }
}
