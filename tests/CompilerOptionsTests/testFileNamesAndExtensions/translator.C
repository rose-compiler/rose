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
    typedef std::vector<std::string> Strings;

    // Usage:
    //    translator --command=CMD NAMES [-- SWITCHES...]         -- runs: CMD SWITCHES NAMES
    //    translator NAMES -- SWITCHES                        -- runs ROSE on SWITCHES NAMES
    // NAMES are always de-escaped and created
    Parser p;
    std::string cmd;
    p.with(Switch("command").argument("cmd", anyParser(cmd)));
    Strings args = p.parse(argc, argv).apply().unreachedArgs();

    // Expand the "+SOMETHING+" escapes in all arguments.
    typedef Sawyer::Container::Map<std::string, std::string> Translations;
    Translations map;
    map.insert("+PLUS+",        "+");
    map.insert("+SPACE+",       " ");
    map.insert("+DOLLAR+",      "$");
    map.insert("+STAR+",        "*");
    map.insert("+HASH+",        "#");
    map.insert("+SQUOTE+",      "'");
    map.insert("+DQUOTE+",      "\"");
    map.insert("+DOT+",         ".");
    map.insert("+SLASH+",       "/");
    map.insert("+BSLASH+",      "\\");
    for (Strings::iterator arg = args.begin(); arg != args.end(); ++arg) {
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
        *arg = translated;
    }

    // If there's a '--' argument, then create a stub C source file for each argument before the '--', move them all to the
    // end, and remove the '--'. I.e., if the arguments are "a b -- c d" then create files "a" and "b" and then rearrange the
    // arguments to read "c d a b".  If there's no "--", then create source files for all the arguments but don't rearrange
    // anything.
    Strings::iterator doubleHyphen = std::find(args.begin(), args.end(), "--");
    for (Strings::iterator arg = args.begin(); arg != doubleHyphen; ++arg) {
        std::ofstream of(arg->c_str());
        ASSERT_require(!of.bad());
        of <<"int main() {}\n";
    }
    if (doubleHyphen != args.end()) {
        Strings tmp(args.begin(), doubleHyphen);
        tmp.insert(tmp.begin(), ++doubleHyphen, args.end());
        args = tmp;
    }

    // Either the ROSE translator or some other command
    if (cmd.empty()) {
        std::cout <<"translator args are:";
        BOOST_FOREACH (const std::string &arg, args)
            std::cout <<" \"" <<StringUtility::cEscape(arg) <<"\"";
        std::cout <<"\n";
        
        args.insert(args.begin(), argv[0]);
        SgProject *project = frontend(args);
        ASSERT_not_null(project);
        exit(backend(project));
    }
    char const** newArgv = new char const*[args.size()+2];
    newArgv[0] = cmd.c_str();
    for (size_t i=0; i<args.size(); ++i)
        newArgv[i+1] = args[i].c_str();
    newArgv[args.size()+1] = NULL;
    execvp(newArgv[0], (char*const*)newArgv);
    ASSERT_not_reachable("exec failed");
}
