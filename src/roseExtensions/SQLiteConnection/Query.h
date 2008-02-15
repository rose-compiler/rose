#ifndef QUERY_H
#define QUERY_H

#include <dbheaders.h>
#include <Result.h>

class Query {

 private:
  string command;
  int err_type;
  string err_msg;
  sqlite3 *db;

 public:
  Query(sqlite3 *&);
  Query(Query &A) {
    err_msg = A.err_msg;
    err_type = A.err_type;
    db = A.db;
  }
  int execute ();
  int execute(const char *);
  int success ();
  string error ();
  void clear() { command = ""; }
  void set(string c) { command = c; }
  Result *store ();
  Result *store_with_types();
  string preview ();
  string& operator<< (const string&);
  //void operator << (const char *);
};

#endif
