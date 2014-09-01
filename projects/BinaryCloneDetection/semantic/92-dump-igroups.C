// Dumps information about input groups so they can be loaded into another database.
#include "sage3basic.h"
#include "SqlDatabase.h"

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE\n"
              <<"  This command dumps the input groups to standard output as SQL that can be loaded into another database\n"
              <<"  with the psql(1) command.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
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

    // Parse switches
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      "see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+1!=argc) {
        std::cerr <<argv0 <<": missing database URL\n"
                  <<"see \"" <<argv0 <<" --help\" for usage info.\n";
        exit(1);
    }
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno])->transaction();

    // Get the set of commands that created these inputs.  This set is pretty small (usually just one command), so we make no
    // attempt to optimize it.
    size_t ncmds = 0;
    SqlDatabase::StatementPtr stmt0 = tx->statement("select distinct cmd from semantic_inputvalues");
    for (SqlDatabase::Statement::iterator row=stmt0->begin(); row!=stmt0->end(); ++row, ++ncmds) {
        SqlDatabase::StatementPtr stmt1 = tx->statement("select hashkey, begin_time, end_time, notation, command"
                                                                 " from semantic_history where hashkey = ?");
        stmt1->bind(0, row.get<uint64_t>(0));
        SqlDatabase::Statement::iterator cmd = stmt1->begin();
        assert(cmd!=stmt1->end());
        std::cout <<"insert into semantic_history (hashkey, begin_time, end_time, notation, command) values ("
                  <<cmd.get<int64_t>(0) <<", "                                            // hashkey
                  <<cmd.get<int64_t>(1) <<", "                                            // begin_time
                  <<cmd.get<int64_t>(2) <<", "                                            // end_time
                  <<SqlDatabase::escape(cmd.get<std::string>(3), tx->driver()) <<", "     // notation
                  <<SqlDatabase::escape(cmd.get<std::string>(4), tx->driver()) <<");\n";  // command
    }

    // Get the input values
    size_t nivalues = 0;
    std::cout <<"copy semantic_inputvalues (igroup_id, queue_id, pos, val, cmd) from stdin;\n";
    SqlDatabase::StatementPtr stmt2 = tx->statement("select igroup_id, queue_id, pos, val, cmd from semantic_inputvalues");
    for (SqlDatabase::Statement::iterator row=stmt2->begin(); row!=stmt2->end(); ++row, ++nivalues) {
        std::cout <<row.get<int32_t>(0) <<"\t"         // igroup_id
                  <<row.get<int32_t>(1) <<"\t"         // queue_id
                  <<row.get<int32_t>(2) <<"\t"         // pos
                  <<row.get<int64_t>(3) <<"\t"         // val
                  <<row.get<int64_t>(4) <<"\n";        // cmd
    }
    std::cout <<"\\.\n";

    std::cerr <<argv0 <<": dumped " <<StringUtility::plural(nivalues, "input values")
              <<" from " <<StringUtility::plural(ncmds, "commands") <<".\n";
    return 0;
}
