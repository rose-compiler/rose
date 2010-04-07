#include "sage3basic.h"

#include <Connection.h>

using namespace std;

int Connection::lastId = 0;

Connection::Connection()
{
  connected = false;
  db = NULL;
  qry = new Query(db);
  dbName = "default.db";
  lastId = 0;
}

Connection::Connection(string dbN)
{
  connected = false;
  db = NULL;
  qry = new Query(db);
  dbName = dbN;
  lastId = 0;
}

int Connection::open()
{
  int rc;
  rc = sqlite3_open(dbName.c_str(), &db);
  if (rc) {
    cerr << "Can't open database: " << sqlite3_errmsg(db);
    return 2;
  }
  connected = true;
  delete qry;
  qry = new Query(db);
  return 0;
}

sqlite3 * Connection::getDB()
{
  return db;
}

int Connection::drop_db(string dbN)
{
  // TODO: remove named database - no correspondent op found in sqlite
  // using physical removal for now
  close();
  string s = "rm " + dbN + "\n";
  system(s.c_str());
  return open();
}

int Connection::close()
{
  int r = sqlite3_close(db);
  connected = false;
  return r;
}

Query *Connection::query()
{
  return qry;
}

int Connection::insert_id()
{
  return lastId++;
}

