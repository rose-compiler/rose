// Create table for function properties aggregated over multiple test runs

#include "sage3basic.h"
#include "CloneDetectionLib.h"

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE\n"
              <<"  This command creates a table that contains properties of functions aggregated over multiple tests.\n"
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

    int argno=1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    }

    if (argno+1!=argc)
        usage(0);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno++])->transaction();
    int64_t cmd_id = CloneDetection::start_command(tx, argc, argv, "running tests");

    typedef std::map<int, double> RetProb;
    RetProb retprob = CloneDetection::function_returns_value(tx);
    tx->execute("delete from semantic_aggprops");
    SqlDatabase::StatementPtr stmt = tx->statement("insert into semantic_aggprops (func_id, retprob) values (?, ?)");
    for (RetProb::iterator rpi=retprob.begin(); rpi!=retprob.end(); ++rpi)
        stmt->bind(0, rpi->first)->bind(1, rpi->second)->execute();

    CloneDetection::finish_command(tx, cmd_id,
                                   "updated aggregate properties for " + StringUtility::plural(retprob.size(), "functions"));
    tx->commit();
    return 0;
}
