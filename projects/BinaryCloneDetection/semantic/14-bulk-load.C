// Knows how to bulk-load a database table efficiently from comma-separated values

#include "SqlDatabase.h"
#include "stringify.h"
#include <fstream>
#include <iostream>

std::string argv0;
static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE TABLE [CSV]\n"
              <<"  This command reads comma-separated values from the CSV file and inserts them into the specified TABLE\n"
              <<"  of the DATABASE.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    TABLE\n"
              <<"            The name of the table where the values will be inserted.\n"
              <<"    CSV\n"
              <<"            The name of the file containing the comma-separated values. Each row of the file corresponds to\n"
              <<"            one row to insert into the table.  If the CSV file is omitted from the command-line, then standard\n"
              <<"            input is read instead.\n";
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
    if (argc!=3 && argc!=4)
        usage(1);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[1])->transaction();
    std::string tablename = argv[2];

    if (argc>=4) {
        std::ifstream in(argv[3]);
        tx->bulk_load(tablename, in);
    } else {
        tx->bulk_load(tablename, std::cin);
    }
    tx->commit();
    return 0;
}
