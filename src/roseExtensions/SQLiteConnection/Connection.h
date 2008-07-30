#ifndef CONNECTION_H
#define CONNECTION_H

#include <dbheaders.h>
#include <Result.h>
#include <Query.h>

class Connection {
 private:
    bool connected;
    std::string dbName;
    sqlite3 *db;
    Query *qry;
    static int lastId;

 public:
    Connection ();
    Connection (std::string dbN);

    int open();
    int close();
    int drop_db(std::string);
    Query *query();
    int insert_id();
    sqlite3 * getDB();
};

#endif

