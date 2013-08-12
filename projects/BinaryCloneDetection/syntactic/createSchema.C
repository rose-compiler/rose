#include "rose.h"
#include "SqlDatabase.h"
#include "createSignatureVectors.h"

int
main(int argc, char *argv[])
{
    if (2!=argc) {
        std::cerr <<"usage: " <<argv[0] <<" DATABASE_NAME\n";
        exit(1);
    }

    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[1])->transaction();
    createDatabases(tx);
    tx->commit();
    return 0;
}
