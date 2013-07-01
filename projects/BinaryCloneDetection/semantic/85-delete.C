// Deletes certain data from the database.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include <cerrno>

using namespace CloneDetection;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE [SPECIFICATION...]\n"
              <<"    SPECIFICATIONS\n"
              <<"       tests [IGROUP_IDS...] [[for] functions FUNCTION_IDS\n"
              <<"            Delete tests for the specified input groups (or all input groups if none are specified) for\n"
              <<"            only the functions whose IDs are specified (or all functions if the \"for functions\" clause\n"
              <<"            is not specified.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

static std::vector<int>
parse_id_list(std::list<std::string> &input/*in,out*/)
{
    std::vector<int> retval;
    while (!input.empty()) {
        std::string arg = input.front();
        std::vector<std::string> words = StringUtility::split(",", arg, (size_t)-1, true);
        for (std::vector<std::string>::iterator wi=words.begin(); wi!=words.end(); ++wi) {
            const char *s = wi->c_str();
            char *rest;
            errno = 0;
            int id = strtol(s, &rest, 0);
            if (errno || rest==s || *rest) {
                if (wi!=words.begin() && words.size()>1) {
                    input.pop_front();
                    input.push_front(StringUtility::join_range(",", wi, words.end()));
                }
                return retval;
            }
            retval.push_back(id);
            input.pop_front();
        }
    }
    return retval;
}

template<class Container>
static std::string
in(const Container &values)
{
    assert(!values.empty());
    std::string retval = "in (";
    for (typename Container::const_iterator vi=values.begin(); vi!=values.end(); ++vi)
        retval += (vi==values.begin()?"":", ") + StringUtility::numberToString(*vi);
    retval += ")";
    return retval;
}

int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno>=argc)
        usage(1);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno++])->transaction();
    int64_t cmd_id = start_command(tx, argc, argv, "deleteting data");

    if (argno>=argc) {
        std::cerr <<argv0 <<": no subcommand specified (see --help)\n";
        exit(1);
    }
    std::string cmd = StringUtility::join_range(" ", argv+argno, argv+argc);
    std::string what = argv[argno++];
    std::list<std::string> input(argv+argno, argv+argc);

    if (0==what.compare("test") || 0==what.compare("tests")) {
        std::vector<int> func_ids, igroup_ids = parse_id_list(input);
        if (!input.empty() && 0==input.front().compare("for"))
            input.pop_front();
        if (!input.empty() && (0==input.front().compare("function") || 0==input.front().compare("functions"))) {
            input.pop_front();
            func_ids = parse_id_list(input);
        }
        if (!input.empty()) {
            std::cerr <<argv0 <<": syntax error in command: " <<cmd <<"\n";
            exit(1);
        }

        std::string cond;
        if (!igroup_ids.empty())
            cond += "igroup_id " + in(igroup_ids);
        if (!func_ids.empty())
            cond += (cond.empty()?"":" and ") + std::string(" func_id ") + in(func_ids);
        if (!cond.empty())
            cond = " where " + cond;

        if (igroup_ids.empty() && func_ids.empty())
            tx->execute("delete from semantic_outputvalues");
        tx->execute("delete from semantic_fio_trace" + cond);
        tx->execute("delete from semantic_fio" + cond);

    } else {
        std::cerr <<argv0 <<": unknown command: " <<cmd <<"\n";
        exit(1);
    }
    
    finish_command(tx, cmd_id);
    tx->commit();
    return 0;
}
