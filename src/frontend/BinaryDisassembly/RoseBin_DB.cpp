/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : General DB connection for MySQL
 ****************************************************/
#include "rose.h"
#include "RoseBin_DB.h"
#include <mysql.h>
//#include "mysql_include/mysql.h"

using namespace std;


/****************************************************
 * Checking for connection problems during a query
 ****************************************************/
void RoseBin_DB::checkError(MYSQL* conn, MYSQL_RES* res_set) {

  if (mysql_errno(conn) !=0)
    cerr << "mysql_fetch_row() failed" << endl;
  else
    if (RoseBin_support::DEBUG_MODE())
    cout << mysql_num_rows(res_set) << " rows returned. " << endl;
  
  ROSE_ASSERT(res_set);
  mysql_free_result(res_set);
}

/****************************************************
 * debugging information on mysql failure
 ****************************************************/
void RoseBin_DB::print_problemWithResults(MYSQL* conn) {
  // no result returned
  if (mysql_field_count(conn)>0) {
    cerr << "Problem processing result set" << endl;
  } else {
    if (RoseBin_support::DEBUG_MODE())
    cout << mysql_affected_rows(conn) << " rows affected. " << endl;
  }
}

/****************************************************
 * processing the result set. Only used for user 
 * defined queries
 ****************************************************/
void RoseBin_DB::process_result_set(MYSQL *conn, MYSQL_RES *res_set) {
  MYSQL_ROW row;
  unsigned int i;
  while ((row = mysql_fetch_row(res_set))!=NULL) {
    for (i=0; i<mysql_num_fields(res_set);i++) {
      if (i>0)
	cout << "\t" ;
      cout << (row[i] !=NULL ? row[i] : "NULL") ;
    }
    cout << endl;
  }
  checkError(conn, res_set);
}

/****************************************************
 * Process a query and return the result set
 ****************************************************/
MYSQL_RES* RoseBin_DB::process_query(MYSQL *conn, char *query) {
  MYSQL_RES *res_set;
  //unsigned int field_count;
  if (mysql_query(conn, query)!=0) {
    cerr << "process_query() failed" << endl;
    exit(1);
  }
  res_set = mysql_store_result(conn);
  return res_set;
}

/****************************************************
 * connect to the DB using the defined parameters
 * at the top of the file
 ****************************************************/
MYSQL* RoseBin_DB::connect_DB(const char* socket) {
  MYSQL* conn = mysql_init (NULL);
  if (conn == NULL) {
    cerr << "mysql_init() failed (probably out of memory)" << endl;
    exit(1);
  } else {
    if (RoseBin_support::DEBUG_MODE())
    cout << "Initialization complete ... " << endl;
  }

  //const char* socket = "/home/panas2/mysql/mysql.sock";
  if (mysql_real_connect(
                         conn,
			 def_host_name,
			 def_user_name,
			 def_password,
			 def_db_name,
			 0,
			 socket, // NULL,
			 0) == NULL) {
    cerr << "mysql_real_connet() failed\nError: " << mysql_error(conn) << endl;
    exit(1);
  } else {
    if (RoseBin_support::DEBUG_MODE())
    cout << "Connection to DB successfull. " << endl;
  }
  return conn;
}






