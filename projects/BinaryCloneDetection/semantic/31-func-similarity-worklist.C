// Generates a list of function pairs whose similarity needs to be computed.

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include <cerrno>

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE\n"
              <<"  This command generates a list of function pairs whose similarity value needs to be computed.  The list\n"
              <<"  is emitted on the standard output stream one pair at a time.  Each line is either a comment (first\n"
              <<"  character is a '#'), blank, or a pair containing two function IDs separated by white space.  The output\n"
              <<"  from this command is typically partitioned into multiple sets of lines and fed into 32-func-similarity\n"
              <<"  commands running in parallel.\n"
              <<"\n"
              <<"    --[no-]delete\n"
              <<"            This switch causes all previous similarity information for the specified relation ID to be\n"
              <<"            discarded so that it is recalculated from scratch. The default is to calculate similarity values\n"
              <<"            only for those pairs of functions in the specified relationship for which similarity has not\n"
              <<"            been calculated already.\n"
              <<"    --exclude-functions=TABLE[.COLUMN]\n"
              <<"            Do not include any of the mentioned functions in the worklist.  The COLUMN defaults to\n"
              <<"            \"func_id\" and should contain ID numbers for functions that should be excluded from the\n"
              <<"            worklist.\n"
              <<"    --relation=ID\n"
              <<"            An integer that identifies which similarity values are being selected.  All similarity values\n"
              <<"            that have the same relation ID form a single similarity relationship.  This allows the database\n"
              <<"            to store multiple relationships. For example, relationship #1 could be similarity values that\n"
              <<"            are calculated from the maximum output group Jaccard index, while relationship #2 could be\n"
              <<"            similarity values that are calculated using a threshold of output group equality count. The\n"
              <<"            default relation ID is zero.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

static struct Switches {
    Switches()
        : delete_old_data(false), relation_id(0) {}
    bool delete_old_data;
    std::string exclude_functions_table;
    int relation_id;
} opt;

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
        } else if (!strcmp(argv[argno], "--delete")) {
            opt.delete_old_data = true;
        } else if (!strncmp(argv[argno], "--exclude-functions=", 20)) {
            opt.exclude_functions_table = argv[argno]+20;
        } else if (!strcmp(argv[argno], "--no-delete")) {
            opt.delete_old_data = false;
        } else if (!strncmp(argv[argno], "--relation=", 11)) {
            opt.relation_id = strtol(argv[argno]+11, NULL, 0);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+1!=argc)
        usage(1);
    time_t start_time = time(NULL);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    SqlDatabase::TransactionPtr tx = conn->transaction();

    // Save ourself in the history if we're modifying the database.
    int64_t cmd_id=-1;
    if (opt.delete_old_data)
        cmd_id = CloneDetection::start_command(tx, argc, argv, "clearing funcsim data for relation #"+
                                               StringUtility::numberToString(opt.relation_id), start_time);

    // The 32-func-similarity tool needs this index, so we might as well create it here when we're running serially.  The
    // semantic_outputvalues table can be HUGE depending on how the analysis is configured (i.e., whether it saves output
    // values as a vector or set, whether it saves function calls and system calls, etc.).  Since creating the index could take
    // a few minutes, we'd rather not create it if it alread exists, but PostgreSQL v8 doesn't have a "CREATE INDEX IF NOT
    // EXISTS" ability.  Therefore, try to create the index right away before we make any other changes, and if creation fails
    // then start a new transaction (because the current one is hosed).
    std::cerr <<argv0 <<": creating output group index (could take a while)\n";
    try {
        SqlDatabase::TransactionPtr tx = conn->transaction();
        tx->execute("create index idx_ogroups_hashkey on semantic_outputvalues(hashkey)");
        tx->commit();
    } catch (const SqlDatabase::Exception&) {
        std::cerr <<argv0 <<": idx_ogroups_hashkey index already exists; NOT dropping and recreating\n";
    }

    // Delete old data.
    if (opt.delete_old_data)
        tx->statement("delete from semantic_funcsim where relation_id = ?")->bind(0, opt.relation_id)->execute();

    // Get the list of functions that should appear in the worklist.
    std::cerr <<argv0 <<": obtaining function list\n";
    std::string stmt1 = "create temporary table tmp_tested_funcs as"
                        " select distinct fio.func_id as func_id"
                        " from semantic_fio as fio";
    if (!opt.exclude_functions_table.empty()) {
        std::vector<std::string> parts = StringUtility::split('.', opt.exclude_functions_table, 2, true);
        if (parts.size()<2)
            parts.push_back("func_id");
        stmt1 += " left join " + parts.front() + " as exclude"
                 " on fio.func_id = exclude." + parts.back() +
                 " where exclude." + parts.back() + " is null";
    }
    tx->execute(stmt1);

    // Create pairs of function IDs for those functions which have been tested and for which no similarity measurement has been
    // computed.  (FIXME: We should probably recompute similarity that might have changed due to rerunning tests or running the
    // same function but with more input groups. [Robb P. Matzke 2013-06-19])
    std::cerr <<argv0 <<": creating work list\n";
    SqlDatabase::StatementPtr stmt2 = tx->statement("select distinct f1.func_id as func1_id, f2.func_id as func2_id"
                                                    " from tmp_tested_funcs as f1"
                                                    " join tmp_tested_funcs as f2 on f1.func_id < f2.func_id"
                                                    " except"
                                                    " select func1_id, func2_id from semantic_funcsim as sim"
                                                    " where sim.relation_id = ?");
    stmt2->bind(0, opt.relation_id);
    for (SqlDatabase::Statement::iterator row=stmt2->begin(); row!=stmt2->end(); ++row)
        std::cout <<row.get<int>(0) <<"\t" <<row.get<int>(1) <<"\n";

    if (cmd_id>=0)
        CloneDetection::finish_command(tx, cmd_id, "cleared funcsim table for relation #"+
                                       StringUtility::numberToString(opt.relation_id));

    tx->commit();
    return 0;
}
