#include "rose.h"
#include "SqlDatabase.h"
#include "createSignatureVectors.h"
#include "../semantic/CloneDetectionLib.h"

static std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<"[--drop] [--] DATABASE\n"
              <<"  This command drops and optionally recreates the schema for syntactic clone detection.\n"
              <<"\n"
              <<"    --drop\n"
              <<"            Drops the database tables but does not create new tables. The default is to drop and then create.\n"
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

    bool drop_only = false;
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--drop")) {
            drop_only = true;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);

    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno])->transaction();

    dropDatabases(tx);
    if (!drop_only)
        createDatabases(tx);
    
    tx->commit();
    return 0;
}
