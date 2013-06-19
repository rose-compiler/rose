// This is mostly just for Robb's PostgreSQL databases. It grants select privilege to role argv[2] for all tables named like
// 'semantic_%'.  Granting access on a per-relation basis is necessary because (1) we're avoiding the use of schemas for
// portability with SQLite3, and (2) even if we did have schemas, the "on all tables of schema" clause wasn't added to
// PostgreSQL until 9.x.

#include "rose.h"
#include "SqlDatabase.h"

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE ROLE\n"
              <<"  This command queries a PostgreSQL database and adds \"select\" privilege to the specified ROLE for all the\n"
              <<"  tables whose names begin with the string \"semantic_\" and possibly a few other things.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    ROLE\n"
              <<"            Name of the PostgreSQL role to which permissions are granted.\n";
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

    // Parse command-line switches
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help\n";
            exit(1);
        }
    }
    if (argno+2!=argc)
        usage(1);
    std::string conn_spec = argv[argno++];
    std::string role = argv[argno++];

    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(conn_spec)->transaction();

    // Get list of table names
    SqlDatabase::Table<std::string, std::string> tables(tx->statement("select tablename, tableowner from pg_catalog.pg_tables"
                                                                      " where tablename like 'semantic_%'"));
    if (tables.empty()) {
        std::cerr <<"no tables?\n";
        return 0;
    }
    tables.headers("Table Name", "Owner");
    tables.print(std::cout);
    std::cout <<"Grant 'select' on these tables to role '" <<role <<"'? [Y/n] ";
    char buf[256];
    std::cin.getline(buf, sizeof buf);
    if ('y'!=buf[0] && 'Y'!=buf[0] && '\0'!=buf[0]) {
        std::cout <<"aborted.\n";
        return 0;
    }

    // Grant privilege to role
    for (size_t i=0; i<tables.size(); ++i)
        tx->execute("grant select on "+tables[i].v0+" to "+role);

    // No need to record this command in semantic_history because we haven't actually modified any data.
    tx->commit();
    return 0;
}
