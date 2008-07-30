#ifndef QUERY_H
#define QUERY_H

#include <dbheaders.h>
#include <Result.h>

class Query {

 private:
  std::string command;
  int err_type;
  std::string err_msg;
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
  std::string error ();
  void clear() { command = ""; }
  void set(const std::string& c) { command = c; }
  Result *store ();
  Result *store_with_types();
  std::string preview ();
  std::string& operator<< (const std::string&);
  //void operator << (const char *);
};

#endif
