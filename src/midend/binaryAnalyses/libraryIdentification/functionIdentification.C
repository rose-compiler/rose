// rose is required because libraryIdentification.h refeences IR nodes.
#include "rose.h"
#include "libraryIdentification.h"

// The cdoe that was here is not in libraryIdentification.h
// #include "functionIdentification.h"

// Note that no special -I options are required to use SSL (so it seems).
#include <openssl/md5.h>

// #include <stdio.h>
// #include <string.h>

// Used for conversions of types to and from strings.
#include <boost/lexical_cast.hpp>

// For debugging, this allows us to alternatively skip the MD5 checksum.
#define USE_MD5_AS_HASH 1

// If no SSL support is available, then save the opcode vector into the database
// (by skipping the generation of an MD5 checksum of the vector).
#if ( (USE_ROSE_SSL_SUPPORT == 0) && (USE_MD5_AS_HASH != 0) )
   #warning "SSL is unavailable so resetting USE_MD5_AS_HASH to false..."
   #undef USE_MD5_AS_HASH
   #define USE_MD5_AS_HASH 0
#endif

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
 
  try {
    con.executenonquery("create index if not exists vectors_by_md5 on vectors(md5_sum)");
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }

};


//Add an entry to store the pair <library_handle,string> in the database
void 
FunctionIdentification::set_function_match( const library_handle & handle, const unsigned char* str, size_t str_length  )
{
  string db_select_n = "INSERT INTO vectors( file, function_name, begin, end, md5_sum ) VALUES(?,?,?,?,?)";

  //construct the entry that is inserted into the database
#if USE_MD5_AS_HASH
  unsigned char md[16];
  MD5( str , str_length, md );
#endif

  sqlite3_command cmd(con, db_select_n.c_str());
  cmd.bind(1, handle.filename );
  cmd.bind(2, handle.function_name );
  cmd.bind(3, (long long int)handle.begin);
  cmd.bind(4, (long long int)handle.end);

// Permit ignoring the MD5 generation
#if USE_MD5_AS_HASH
  cmd.bind(5, md,16);
#else
  cmd.bind(5, str,str_length);
#endif

  cmd.executenonquery();

};

// Interface to lower level function taking unsigned char* and length
void 
FunctionIdentification::set_function_match( const library_handle & handle, const std::string functionString  )
   {
     set_function_match( handle, (const unsigned char*)functionString.c_str() , functionString.size() );
   }

// Interface to lower level function taking unsigned char* and length
void 
FunctionIdentification::set_function_match( const library_handle & handle, const SgUnsignedCharList & opcode_vector )
   {
     set_function_match( handle, (const unsigned char*) &(*(opcode_vector.begin())) , opcode_vector.size() );
   }



//Return the library_handle matching string from the database. bool false
//is returned if no such match was found, true otherwise.
bool 
FunctionIdentification::get_function_match( library_handle & handle, const unsigned char* str, size_t str_length )
{
  //Constructing query command to get the entry from the database
#if USE_MD5_AS_HASH
  unsigned char md[16];
  MD5( str , str_length, md );
#endif

  std::string db_select_n = "select file, function_name, begin,end from vectors where md5_sum=?";
//    +boost::lexical_cast<string>(md) + "'";
  sqlite3_command cmd(con, db_select_n );

// Permit ignoring the MD5 generation
#if USE_MD5_AS_HASH
  cmd.bind(1,md,16);
#else
  cmd.bind(1,str,str_length);
#endif

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

bool 
FunctionIdentification::get_function_match( library_handle & handle, const std::string functionString) const
   {
     return const_cast<FunctionIdentification*>(this)->get_function_match( handle, (const unsigned char*)functionString.c_str() , functionString.size() );
   }

// Interface to lower level function taking unsigned char* and length
bool
FunctionIdentification::get_function_match( library_handle & handle, const SgUnsignedCharList & opcode_vector ) const
   {
     return const_cast<FunctionIdentification*>(this)->get_function_match( handle, (const unsigned char*) &(*(opcode_vector.begin())) , opcode_vector.size() );
   }

