#include "functionIdentification.h"
#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>
#include <boost/lexical_cast.hpp>


using namespace std;
using namespace sqlite3x;
using namespace LibraryIdentification;

FunctionIdentification::FunctionIdentification(std::string dbName)
{
  database_name = dbName;
  //open the database
  con.open(database_name.c_str());
  con.setbusytimeout(1800 * 1000); // 30 minutes
  createTables();
};

//Make sure all functions are defined in the function identification database
void 
FunctionIdentification::createTables()
{
  try {
    con.executenonquery("create table IF NOT EXISTS vectors(row_number INTEGER PRIMARY KEY, file TEXT, function_name TEXT,  begin INTEGER, end INTEGER, md5_sum TEXT)");
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }
 
};


//Add an entry to store the pair <library_handle,string> in the database
void 
FunctionIdentification::set_function_match( library_handle handle, std::string functionString  )
{
  string db_select_n = "INSERT INTO vectors( file, function_name, begin, end, md5_sum ) VALUES(?,?,?,?,?)";

  //construct the entry that is inserted into the database
  unsigned char md[16];
  MD5( (const unsigned char*)functionString.c_str() , functionString.size(), md ) ;
  sqlite3_command cmd(con, db_select_n.c_str());
  cmd.bind(1, handle.filename );
  cmd.bind(2, handle.function_name );
  cmd.bind(3, (long long int)handle.begin);
  cmd.bind(4, (long long int)handle.end);
  cmd.bind(5, md,16);

  cmd.executenonquery();

};
//Return the library_handle matching string from the database. bool false
//is returned if no such match was found, true otherwise.
bool 
FunctionIdentification::get_function_match(library_handle& handle, std::string functionString)
{
  //Constructing query command to get the entry from the database
  unsigned char md[16];
  MD5( (unsigned const char*)functionString.c_str() , functionString.size(), md ) ;
  std::string db_select_n = "select file, function_name, begin,end from vectors where md5_sum=?";
//    +boost::lexical_cast<string>(md) + "'";
  sqlite3_command cmd(con, db_select_n );
  cmd.bind(1,md,16);

  sqlite3_reader r = cmd.executereader();


  if(r.read()){ //entry found in database
    handle.filename = (std::string)(r.getstring(0));
    handle.function_name = r.getstring(1);
    handle.begin = boost::lexical_cast<size_t>(r.getstring(2));
    handle.end   = boost::lexical_cast<size_t>(r.getstring(3));


    //Only one entry should exist
    if(r.read()) 
    {
      std::cerr << "Duplicate entries for " << handle.filename << " " << handle.function_name << " " << handle.begin << " " 
        << handle.end << " in the database. Exiting."  << std::endl;
      return false;
    }else 
      return true;
  }

  return false;


};

