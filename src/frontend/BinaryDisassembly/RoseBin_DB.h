/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code for arbitrary DB connection
 ****************************************************/

#ifndef __RoseBin_DB__
#define __RoseBin_DB__

#include <mysql.h>
//#include <mysql_include/mysql.h>

#include <stdio.h>
#include <iostream>
// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "rose.h"
// #include "RoseBin_support.h"


class RoseBin_DB{
 protected:
  const char* def_host_name; 
  const char* def_user_name;
  const char* def_password;
  const char* def_db_name;

 public:


  RoseBin_DB() {};
  ~RoseBin_DB() {
  }


  /****************************************************
   * debugging information on mysql failure
   ****************************************************/
  void print_problemWithResults(MYSQL* conn);

  /****************************************************
   * Checking for connection problems during a query
   ****************************************************/
  void checkError(MYSQL* conn, MYSQL_RES* res_set);


  /****************************************************
   * processing the result set. Only used for user 
   * defined queries
   ****************************************************/
  void process_result_set(MYSQL *conn, MYSQL_RES *res_set);

  /****************************************************
   * Process a query and return the result set
   ****************************************************/
  MYSQL_RES* process_query(MYSQL *conn, char *query);

  /****************************************************
   * connect to the DB using the defined parameters
   * at the top of the file
   ****************************************************/
  MYSQL* connect_DB(const char* socket);

};

#endif

