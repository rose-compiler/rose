#include <rose.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <fstream>
#include <Sawyer/CommandLine.h>
#include <unistd.h>                                     // execvp

int
main(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    // Usage:
    //    translator --command=CMD NAMES [-- SWITCHES...]         -- runs: CMD SWITCHES NAMES
    //    translator NAMES -- SWITCHES                        -- runs ROSE on SWITCHES NAMES
    // NAMES are always de-escaped and created
    Parser p;
    std::string cmd;
    p.with(Switch("command").argument("cmd", anyParser(cmd)));
    std::vector<std::string> args = p.parse(argc, argv).apply().unreachedArgs();

    // Escape file names (everything up to the next "--" or the end of the arguments). Escape sequences have a syntax that
    // makes them easy to pass through makefiles and shell scripts:
    typedef Sawyer::Container::Map<std::string, std::string> Translations;
    Translations map;
    map.insert("+PLUS+",        "+");
    map.insert("+SPACE+",       " ");
    map.insert("+DOLLAR+",      "$");
    map.insert("+STAR+",        "*");
    map.insert("+HASH+",        "#");
    map.insert("+SQUOTE+",      "'");
    map.insert("+DQUOTE+",      "\"");

    std::vector<std::string> translatedArgs;
    for (std::vector<std::string>::iterator arg = args.begin(); arg != args.end(); ++arg) {
        if (*arg == "--") {
            // The rest of the args (other than the "--") are moved to the beginning of the translated args.
            translatedArgs.insert(translatedArgs.begin(), ++arg, args.end());
            break;
        } else {
            std::string translated;
            for (size_t i=0; i<arg->size(); ++i) {
                size_t endPlus;
                if ((*arg)[i] == '+' && (endPlus=arg->find('+', i+1)) != std::string::npos) {
                    std::string token = arg->substr(i, endPlus+1-i);
                    translated += map.getOrElse(token, token);
                    i = endPlus;
                } else {
                    translated += (*arg)[i];
                }
            }
            translatedArgs.push_back(translated);

            // Create nearly empty source files
            std::ofstream of(translated.c_str());
            ASSERT_require(!of.bad());
            of <<"int main() {}\n";
        }
    }

    // Either the ROSE translator or some other command
    if (cmd.empty()) {
        std::cout <<"translator args are:";
        BOOST_FOREACH (const std::string &arg, translatedArgs)
            std::cout <<" \"" <<StringUtility::cEscape(arg) <<"\"";
        std::cout <<"\n";
        
        translatedArgs.insert(translatedArgs.begin(), argv[0]);
        SgProject *project = frontend(translatedArgs);
        ASSERT_not_null(project);
        backend(project);
    } else {
        char const** newArgv = new char const*[translatedArgs.size()+2];
        newArgv[0] = cmd.c_str();
        for (size_t i=0; i<translatedArgs.size(); ++i)
            newArgv[i+1] = translatedArgs[i].c_str();
        newArgv[translatedArgs.size()+1] = NULL;
        execvp(newArgv[0], (char*const*)newArgv);
        ASSERT_not_reachable("exec failed");
    }
}
