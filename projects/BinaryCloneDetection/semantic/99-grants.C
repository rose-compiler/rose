// This is mostly just for Robb. It grants select privilege to role argv[2] for all tables named like 'semantic_%'
// Works only for PostgreSQL (SQLite3 permissions are from the file system).
#include "rose.h"
#include "SqlDatabase.h"

int
main(int argc, char *argv[])
{
    assert(3==argc);
    std::string conn_spec = argv[1];
    std::string role = argv[2];

    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(conn_spec)->transaction();
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

    for (size_t i=0; i<tables.size(); ++i)
        tx->execute("grant select on "+tables[i].v0+" to "+role);

    tx->commit();
    return 0;
}
