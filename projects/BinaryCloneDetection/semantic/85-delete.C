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
              <<"       tests [IGROUP_IDS...] [[for] functions FUNCTION_IDS]\n"
              <<"            Delete tests for the specified input groups (or all input groups if none are specified) for\n"
              <<"            only the functions whose IDs are specified (or all functions if the \"for functions\" clause\n"
              <<"            is not specified). This does not delete results that depend on multiple tests, such as partial\n"
              <<"            analysis results or function similarities.\n"
              <<"       partials [[for] functions FUNCTION_IDS]\n"
              <<"            Remove the partial analysis results for all functions (or the specified functions). Partial\n"
              <<"            results are the analyses that run during 25-run-tests but which are not stored in their final\n"
              <<"            form in the database.  For example, the analysis that tries to determine the likelihood that\n"
              <<"            a function returns a value is based on tests of the call sites for that function, and those\n"
              <<"            call sites could be tested in (many) other instances of 25-run-tests; the final answer\n"
              <<"            depends on the results from all those tests and is therefore not stored in the database.\n"
              <<"       inputs [IGROUP_IDS...]\n"
              <<"            Deletes the specified input groups and all tests that depended on those input groups.  This does\n"
              <<"            not delete subsquent results that depend on multiple inputs (such as function similarities).\n"
              <<"       results [[for] function FUNCTION_IDS]\n"
              <<"            Delete all testing results (or results for the specified functions). This the tests, the outputs\n"
              <<"            and traces produced by those tests, the partial analysis results, and function similarity.\n"
              <<"            Basically, everything that depends on results from 25-run-tests, but nothing that depends\n"
              <<"            only on the lower-numbered commands.\n"
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
        }
        input.pop_front();
    }
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
            cond += "igroup_id " + SqlDatabase::in(igroup_ids);
        if (!func_ids.empty())
            cond += (cond.empty()?"":" and ") + std::string(" func_id ") + SqlDatabase::in(func_ids);
        if (!cond.empty())
            cond = " where " + cond;

        if (igroup_ids.empty() && func_ids.empty())
            tx->execute("delete from semantic_outputvalues");
        tx->execute("delete from semantic_fio_calls" + cond);
        tx->execute("delete from semantic_fio_coverage" + cond);
        tx->execute("delete from semantic_fio_inputs" + cond);
        tx->execute("delete from semantic_fio_trace" + cond);
        tx->execute("delete from semantic_fio" + cond);

    } else if (0==what.compare("partials")) {
        std::vector<int> func_ids;
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

        std::string sql = "delete from semantic_funcpartials";
        if (!func_ids.empty())
            sql += " where func_id " + SqlDatabase::in(func_ids);
        tx->execute(sql);

    } else if (0==what.compare("input") || 0==what.compare("inputs") || 0==what.compare("igroup") || 0==what.compare("igroups")) {
        std::vector<int> igroup_ids = parse_id_list(input);
        if (!input.empty()) {
            std::cerr <<argv0 <<": syntax error in command: " <<cmd <<"\n";
            exit(1);
        }

        std::string cond = igroup_ids.empty() ? std::string() : " where igroup_id " + SqlDatabase::in(igroup_ids);
        tx->execute("delete from semantic_fio_calls" + cond);
        tx->execute("delete from semantic_fio_coverage" + cond);
        tx->execute("delete from semantic_fio_inputs" + cond);
        tx->execute("delete from semantic_fio_trace" + cond);
        tx->execute("delete from semantic_fio" + cond);
        tx->execute("delete from semantic_inputvalues" + cond);

    } else if (0==what.compare("result") || 0==what.compare("results")) {
        std::vector<int> func_ids;
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

        std::string funcs = func_ids.empty() ? std::string("is not null") : SqlDatabase::in(func_ids);
        tx->execute("delete from semantic_funcsim where func1_id "+funcs+" or func2_id "+funcs);
        tx->execute("delete from semantic_funcpartials where func_id "+funcs);
        tx->execute("delete from semantic_fio_calls where func_id "+funcs);
        tx->execute("delete from semantic_fio_coverage where func_id "+funcs);
        tx->execute("delete from semantic_fio_inputs where func_id "+funcs);
        tx->execute("delete from semantic_fio_trace where func_id "+funcs);
        tx->execute("delete from semantic_fio where func_id "+funcs);
    } else {
        std::cerr <<argv0 <<": unknown command: " <<cmd <<"\n";
        exit(1);
    }

    finish_command(tx, cmd_id);
    tx->commit();
    return 0;
}
