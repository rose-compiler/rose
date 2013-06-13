// Returns the list of tests that remain to be executed.  Each line of output is one test and contains the following
// tab-separated fields:
//   0. The ID number of the function
//   1. The input group number

#include "sage3basic.h"
#include "CloneDetectionLib.h"

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE\n"
              <<"  This command produces a list of tests that are not present in the database. Each line of output\n"
              <<"  corresponds to one test and contains the file ID for the specimen, the function ID, and the input\n"
              <<"  group number separated from one another by TAB characters.\n"
              <<"\n"
              <<"    --entry=ADDR\n"
              <<"            Select only functions having the specified entry address, which may be specified in decimal\n"
              <<"            hexadecimal, or octal using the usual C syntax.  This switch may appear more than once to select\n"
              <<"            functions that have any of the specified addresses.\n"
              <<"    --name=NAME\n"
              <<"            Select only functions having the specified names.  This switch may appear more than once to\n"
              <<"            select functions that have any of the specified names.\n"
              <<"    --nfuzz=N\n"
              <<"            Restricts the number of input groups to be at most N.  The default is to consider all existing\n"
              <<"            input groups.\n"
              <<"    --first-fuzz=N\n"
              <<"            Lowest numbered fuzz test to consider.  The default is zero."
              <<"    --size=NINSNS\n"
              <<"            Select only functions that have at least NINSNS instructions.\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

struct Switches {
    Switches(): ninsns(0), nfuzz(0), first_fuzz(0), nfuzz_set(false) {}
    std::set<rose_addr_t> entry_vas;
    std::set<std::string> names;
    size_t ninsns, nfuzz, first_fuzz;
    bool nfuzz_set; // is nfuzz value valid?
};
    
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

    Switches opt;
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--entry=", 8)) {
            opt.entry_vas.insert(strtoull(argv[argno]+8, NULL, 0));
        } else if (!strncmp(argv[argno], "--name=", 7)) {
            opt.names.insert(argv[argno]+7);
        } else if (!strncmp(argv[argno], "--size=", 7)) {
            opt.ninsns = strtoul(argv[argno]+7, NULL, 0);
        } else if (!strncmp(argv[argno], "--nfuzz=", 8)) {
            opt.nfuzz = strtoul(argv[argno]+8, NULL, 0);
            opt.nfuzz_set = true;
        } else if (!strncmp(argv[argno], "--first-fuzz=", 13)) {
            opt.first_fuzz = strtoul(argv[argno]+13, NULL, 0);
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno++])->transaction();

    // Create table tmp_functions containing IDs for selected functions and their specimen IDs
    std::vector<std::string> constraints;
    if (!opt.entry_vas.empty()) {
        std::string s = "entry_va in (";
        for (std::set<rose_addr_t>::iterator i=opt.entry_vas.begin(); i!=opt.entry_vas.end(); ++i)
            s += (i==opt.entry_vas.begin()?"":", ") + StringUtility::numberToString(*i);
        constraints.push_back(s+")");
    }
    if (!opt.names.empty()) {
        std::string s = "name in (";
        for (std::set<std::string>::iterator i=opt.names.begin(); i!=opt.names.end(); ++i)
            s += (i==opt.names.begin()?"":", ") + SqlDatabase::escape(*i, tx->driver());
        constraints.push_back(s+")");
    }
    if (opt.ninsns>0)
        constraints.push_back("size >= " + StringUtility::numberToString(opt.ninsns));
    std::string sql1 = "select id, specimen_id from semantic_functions";
    for (std::vector<std::string>::iterator i=constraints.begin(); i!=constraints.end(); ++i)
        sql1 += (i==constraints.begin()?" where ":" and ") + *i;
    tx->execute("create temporary table tmp_functions as " + sql1);

    // Create table tmp_inputgroups containing IDs for selected input groups
    std::string sql2 = "select distinct id from semantic_inputvalues where id >= " +
                       StringUtility::numberToString(opt.first_fuzz);
    if (opt.nfuzz_set)
        sql2 += " and id < " + StringUtility::numberToString(opt.first_fuzz+opt.nfuzz);
    tx->execute("create temporary table tmp_inputgroups as " + sql2);

    // Create tmp_pending as the cross product of functions and inputgroups except for those already tested
    tx->execute("create temporary table tmp_pending as"
                "    select func.specimen_id as specimen_id, func.id as func_id, igroup.id as igroup_id"
                "      from tmp_functions as func"
                "      join tmp_inputgroups as igroup"
                "      on igroup.id is not null"        // "on" clause and "is not null" (rather than "true") for portability
                "  except"
                "    select func.specimen_id, func.id, fio.igroup_id"
                "      from semantic_fio as fio"
                "      join semantic_functions as func on fio.func_id=func.id");
    SqlDatabase::StatementPtr stmt = tx->statement("select specimen_id, func_id, igroup_id"
                                                   " from tmp_pending"
                                                   " order by specimen_id, igroup_id, func_id");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row)
        std::cout <<row.get<int>(0) <<"\t" <<row.get<int>(1) <<"\t" <<row.get<int>(2) <<"\n";

    // no need to commit, but if we change this in the future, be sure to add begin_command()/finish_command()
    return 0;
}

