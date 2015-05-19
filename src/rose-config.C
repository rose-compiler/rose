#include <sage3basic.h>
#include <Diagnostics.h>

#include <boost/foreach.hpp>
#include <sawyer/CommandLine.h>
#include <string>
#include <vector>

using namespace rose::Diagnostics;

// The following symbols should be defined on our compile command line:
//   CXX
//      The name of the compiler used to build the ROSE library, from the Makefile $(CXX) variable.
//   BUILD_CPPFLAGS
//      Precompiler switches, such as "-D" and "-I", that are used when building a tool inside the ROSE build tree.
//      This is from the $(ROSE_INCLUDES) makefile variable.
//   INSTALLED_CPPFLAGS
//      Precompiler switches, such as "-D" and "-I", that are used when building a tool outside the ROSE build tree from
//      a version of ROSE that has been installed.
//   CFLAGS
//      Compile switches other than those passed to the preprocessor. E.g., warning flags, optimization switches.
//   BUILD_LDFLAGS
//      Linker switches when linking directly from the ROSE build directories.
//   INSTALLED_LDFLAGS
//      Linker switches when linking to an installed version of ROSE.

enum Mode {
    BUILD,                                              // Emit info for building inside the ROSE build directories
    INSTALLED,                                          // Emit info for building based on an installed version of ROSE
    NMODES
};

struct Config {
    const char *cxx;                                    // C++ compiler command
    const char *cppflags;                               // C preprocessor switches
    const char *cflags;                                 // C++ compiling switches excluding preprocessor switches
    const char *ldflags;                                // Loader/linker switches
};

static Config configs[] = {
    { CXX, BUILD_CPPFLAGS,     CFLAGS, BUILD_LDFLAGS     },
    { CXX, INSTALLED_CPPFLAGS, CFLAGS, INSTALLED_LDFLAGS }
};

struct Settings {
    Mode mode;
    Settings(): mode(INSTALLED) {}
};

// Parse switches and return non-switch arguments
static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;

    Parser parser;
    parser
        .purpose("show ROSE compilation arguments")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}] @v{variable}")
        .doc("Description",
             "This command displays various configuration settings that are useful in user makefiles when compiling or "
             "linking a program that uses ROSE.  Two modes of compiling are available depending on the @s{mode} switch. "
             "The @v{variable} can be any of the following words:"
             "@named{cxx}{Displays the name of the C++ compiler.}"
             "@named{cppflags}{Shows the switches that should be passed to the C preprocessor as part of compile commands.}"
             "@named{cflags}{Shows the compiler switches, excluding preprocessor switches, that should be used when compiling "
             "a program that uses ROSE.}"
             "@named{ldflags}{Shows the compiler switches that should be used when linking a program that uses the ROSE "
             "library.}"
             "@named{libdirs}{Shows a colon-separated list of library directories. These are the directories that might "
             "contain shared libraries.}");

    SwitchGroup switches;
    switches.insert(Switch("help", 'h')
                    .doc("Show this documentation.")
                    .action(showHelpAndExit(0)));

    switches.insert(Switch("version", 'V')
                    .action(showVersionAndExit(version_message(), 0))
                    .doc("Shows version information for various ROSE components and then exits."));

    switches.insert(Switch("mode")
                    .argument("mode", enumParser<Mode>(settings.mode)->with("build", BUILD)->with("installed", INSTALLED))
                    .doc("Specifies the mode for using the ROSE library. The @v{mode} should be \"build\" to "
                         "emit configuration information suitable for building a tool using headers and libraries "
                         "directly from the ROSE build tree, or \"installed\" to emit information suitable for building "
                         "a tool using the installed versions of ROSE headers and libraries.  The default is \"installed\"."));

    return parser.with(switches).parse(argc, argv).apply().unreachedArgs();
}

// Convert linker switches to a colon-separated list of directory names.
static std::string
makeLibrarySearchPaths(const char *s) {
    // Split string into space-separated arguments in a rather simplistic way.
    std::vector<std::string> args;
    char quoted = '\0';
    std::string arg;
    for (/*void*/; s && *s; ++s) {
        if ('\\'==*s && s[1]) {
            arg += *++s;
        } else if ('\''==*s || '"'==*s) {
            if (!quoted) {
                quoted = *s;
            } else if (quoted == *s) {
                quoted = '\0';
            } else {
                arg += *s;
            }
        } else if (isspace(*s)) {
            if (quoted) {
                arg += *s;
            } else if (!arg.empty()) {
                args.push_back(arg);
                arg = "";
            }
        } else {
            arg += *s;
        }
    }

    // Look for "-Lxxx" or "-L xxx" and save the xxx parts in a colon-separated string.
    std::string retval;
    for (size_t i=0; i<args.size(); ++i) {
        if (args[i]=="-L" && i+1<args.size()) {
            retval += (retval.empty()?"":":") + args[++i];
        } else if (args[i].size() > 2 && args[i].substr(0, 2)=="-L") {
            retval += (retval.empty()?"":":") + args[i].substr(2);
        }
    }
    return retval;
}

// Escape special characters for including in a makefile string.
// FIXME[Robb P. Matzke 2015-04-14]: for now, just use C-style escaping, which might be good enough.
static std::string
makefileEscape(const std::string &s) {
    return StringUtility::cEscape(s);
}

int
main(int argc, char *argv[]) {
    rose::Diagnostics::initialize();
    try {
        Settings settings;
        std::vector<std::string> commands = parseCommandLine(argc, argv, settings);
        if (commands.empty())
            throw std::runtime_error("no command specified; see --help");
        ASSERT_require(settings.mode < NMODES);
        const Config &config = configs[settings.mode];

        BOOST_FOREACH (const std::string &command, commands) {
            if (command == "cxx") {
                std::cout <<makefileEscape(config.cxx) <<"\n";
            } else if (command == "cppflags") {
                std::cout <<makefileEscape(config.cppflags) <<"\n";
            } else if (command == "cflags") {
                std::cout <<makefileEscape(config.cflags) <<"\n";
            } else if (command == "ldflags") {
                std::cout <<makefileEscape(config.ldflags) <<"\n";
            } else if (command == "libdirs") {
                std::cout <<makefileEscape(makeLibrarySearchPaths(config.ldflags)) <<"\n";
            } else {
                throw std::runtime_error("unrecognized command: \"" + StringUtility::cEscape(command) + "\"");
            }
        }
    } catch (const std::runtime_error &error) {
        mlog[FATAL] <<error.what() <<"\n";
        exit(1);
    }
}
