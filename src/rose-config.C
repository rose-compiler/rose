#define CONFIG_NAME "rose-config.cfg"

static const char *purpose = "show ROSE configuration arguments";
static const char *description =
    "This command displays various configuration settings that are useful in user makefiles when compiling or linking a "
    "program that uses ROSE.  It does this by reading a configuration file named " CONFIG_NAME " from the directory that "
    "holdes the ROSE libraries, or from the file specified with the @s{config} switch.  This file contains blank lines, "
    "comments starting with '#' as the first non-white-space character of a line, or lines of the form \"KEY = VALUE\" "
    "where white-space around either end of the VALUE is stripped.\n\n"

    "The tool should be invoked with one positional argument: the name of the KEY whose value is to be reported. The "
    "following keys must be present in the " CONFIG_NAME " file:"

    // If you modify this list, be sure to update requiredKeys()
    "@named{cc}{Displays the name of the C compiler.}"

    "@named{cxx}{Displays the name of the C++ compiler.}"

    "@named{cppflags}{Shows the switches that should be passed to the C preprocessor as part of compile commands.}"

    "@named{cflags}{Shows the C compiler switches, excluding preprocessor switches, that should be used when compiling a "
    "program that uses ROSE.}"

    "@named{cxxflags}{Shows the C++ compiler switches, excluding preprocessor switches, that should be used when compiling a "
    "program that uses ROSE.}"

    "@named{ldflags}{Shows the compiler switches that should be used when linking a program that uses the ROSE library.}"

    "@named{libdirs}{Shows a colon-separated list of library directories. These are the directories that might contain shared "
    "libraries.}"

    "@named{prefix}{ROSE installation prefix. This is the name of the directory that includes \"lib\" and \"include\" "
    "subdirectories (among others) where the ROSE library and its headers are installed.}";

#include <rose.h>
#include <Diagnostics.h>
#include <rose_getline.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <Sawyer/CommandLine.h>
#include <map>
#include <string>
#include <vector>

using namespace Rose;
using namespace Sawyer::Message::Common;

typedef std::map<std::string, std::string> Configuration;

Sawyer::Message::Facility mlog;

struct Settings {
    std::string searchDirs;
    boost::filesystem::path configFile;

    // When compiling this program, LIBDIR C preprocessor symbol should be the name of the installation path for libraries.
    Settings()
        : searchDirs(LIBDIR) {}
};

static const std::vector<std::string>&
requiredKeys() {
    static std::vector<std::string> required;
    if (required.empty()) {
        // These should all be documented in the --help output
        required.push_back("cc");
        required.push_back("cxx");
        required.push_back("cppflags");
        required.push_back("cflags");
        required.push_back("cxxflags");
        required.push_back("ldflags");
        required.push_back("libdirs");
        required.push_back("prefix");
    }
    return required;
}

// Parse switches and return the single positional KEY argument.
static std::string
parseCommandLine(int argc, char *argv[], Settings &settings /*in,out*/) {
    using namespace Sawyer::CommandLine;

    Parser parser = CommandlineProcessing::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{variable}");
    parser.with(CommandlineProcessing::genericSwitches());

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("config")
                .argument("file", anyParser(settings.configFile))
                .doc("Use the specified file instead of the " CONFIG_NAME " file installed as part of installing ROSE."));
    parser.with(tool);
                
    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();

    if (args.size() != 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    return args[0];
}

// Read a specific configuration file
static Configuration
readConfigFile(const boost::filesystem::path &configName) {
    struct Resources {
        FILE *file;
        char *line;
        size_t linesz;

        Resources()
            : file(NULL), line(NULL), linesz(0) {}

        ~Resources() {
            if (file)
                fclose(file);
            if (line)
                free(line);
        }
    } r;

    Configuration retval;

    if (NULL == (r.file = fopen(configName.string().c_str(), "r"))) {
        mlog[FATAL] <<strerror(errno) <<": \"" <<configName <<"\"\n";
        exit(1);
    }

    boost::regex keyRe("[a-zA-Z][a-zA-Z_0-9]*");
    size_t lineNumber = 0;
    while (ssize_t nchars = rose_getline(&r.line, &r.linesz, r.file)) {
        ++lineNumber;
        if (nchars < 0) {
            if (errno) {
                mlog[FATAL] <<configName <<":" <<lineNumber <<": " <<strerror(errno) <<"\n";
                exit(1);
            }
            break;                                      // EOF
        }

        std::string s = r.line;
        boost::trim(s);
        if (s.empty() || '#' == s[0])
            continue;

        // Parse the "key=value" line
        size_t equal = s.find('=');
        std::string key = s.substr(0, equal);
        boost::trim(key);
        std::string value = equal == std::string::npos ? std::string() : s.substr(equal+1);
        boost::trim(value);
        if (equal == std::string::npos || !boost::regex_match(key, keyRe)) {
            mlog[FATAL] <<configName <<":" <<lineNumber <<": syntax error: expected KEY = VALUE\n";
            exit(1);
        }

        // Save the key and value for returning later
        if (!retval.insert(std::make_pair(key, value)).second) {
            mlog[FATAL] <<configName <<":" <<lineNumber <<": duplicate key \"" <<StringUtility::cEscape(key) <<"\"\n";
            exit(1);
        }
    }

    bool hadError = false;
    BOOST_FOREACH (const std::string &key, requiredKeys()) {
        if (retval.find(key) == retval.end()) {
            mlog[FATAL] <<configName <<":" <<lineNumber <<": required key \"" <<key <<"\" is not defined\n";
            hadError = true;
        }
    }
    if (hadError)
        exit(1);
            
    return retval;
}

// Read the first configuration file we can find.
static Configuration
readConfigFile(const Settings &settings) {
    if (!settings.configFile.empty())
        return readConfigFile(settings.configFile);

    std::vector<std::string> dirs;
    boost::split(dirs, settings.searchDirs, boost::is_any_of(":;"));
    BOOST_FOREACH (const std::string &dir, dirs) {
        boost::filesystem::path configFile = boost::filesystem::path(dir) / CONFIG_NAME;
        if (boost::filesystem::exists(configFile))
            return readConfigFile(configFile);
    }

    mlog[FATAL] <<"cannot find file \"" <<StringUtility::cEscape(CONFIG_NAME) <<"\"\n";
    mlog[FATAL] <<"searched in these directories:\n";
    BOOST_FOREACH (const std::string &dir, dirs)
        mlog[FATAL] <<"  \"" <<StringUtility::cEscape(dir) <<"\"\n";
    exit(1);
}

// Convert linker switches to a colon-separated list of directory names.
static std::string
makeLibrarySearchPaths(const std::string &str) {
    // Split string into space-separated arguments in a rather simplistic way.
    std::vector<std::string> args;
    {
        char quoted = '\0';
        std::string arg;
        const char *s = str.c_str();
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
        ASSERT_require2(quoted=='\0', "mismatched quotes");
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
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");

    Settings settings;
    std::string key = parseCommandLine(argc, argv, settings);
    Configuration config = readConfigFile(settings);

    // Print the value
    Configuration::const_iterator found = config.find(key);
    if (found != config.end()) {
        if (key == "libdirs") {
            std::cout <<makefileEscape(makeLibrarySearchPaths(found->second)) <<"\n";
        } else {
            std::cout <<makefileEscape(found->second) <<"\n";
        }
        exit(0);
    }

    // Errors
    mlog[FATAL] <<"unknown key \"" <<StringUtility::cEscape(key) <<"\"\n";
    exit(1);
}
