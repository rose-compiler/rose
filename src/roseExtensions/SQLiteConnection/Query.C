#include "sage3basic.h"

#include <Query.h>

using namespace std;

Query::Query (sqlite3 *&db) {
  command = ""  ;
  err_msg = ""  ;
  err_type = 0;
  this->db = db;
}

string& Query::operator << (const string& command) {
  this->command += command;
  return this->command;
  }
/*
void Query::operator << (const char *com) {
  this->command = com;
}
*/
int Query::execute () {
  char *err_m = 0;

  if (command == "")
    return 0;
  //  cout << "Executing: " << command << "\n";
  err_type = sqlite3_exec(db, command.c_str(), NULL, NULL, &err_m);
  if (err_m)
    err_msg = (string)err_m;
  else
    err_msg = "";
  // cout << "Error type: " << err_type << " + message: " << err_m << "\n";
  return err_type;
}

int Query::execute (const char *com) {
  char *err_m = 0;

  if (com == NULL)
    return 0;
  //  cout << "Executing: " << com << "\n";
  err_type = sqlite3_exec(db, com, NULL, NULL, &err_m);
  if (err_m)
    err_msg = (string)err_m;
  else
    err_msg = "";
  //  cout << "Error type: " << err_type << " + message: " << err_m << "\n";
  return err_type;
}

int Query::success () {
  return err_type;
}

string Query::error () {
  return err_msg;
}

string Query::preview () {
  return command;
}

Result *Query::store () {
  char **res;
  char *err_m;
  int nRow = 0, nCol = 0;
  // TODO: no column types for now
  vector <string> *types = NULL;
  
  //cout << "Executing: " << preview() << "\n";
  err_type = sqlite3_get_table(db, command.c_str(), &res, &nRow, &nCol, &err_m);
  if (err_m)
    err_msg = (string)err_m;
  else
    err_msg = "";
  //cout << "passed: " << success() << "  with error: " << error() << "\n";
  Result *r = new Result(res, nRow, nCol, types);
  return r;
}

Result *Query::store_with_types () {
  cout << "Unimplemented \"store_with_types\"!\n";
  return store();
}

