#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"                                 // every librose .C file must start with this

#include "libraryIdentification.h"

// The cdoe that was here is not in libraryIdentification.h
// #include "functionIdentification.h"

// Note that no special -I options are required to use SSL (so it seems).
#include <openssl/md5.h>

// #include <stdio.h>
// #include <string.h>

// Used for conversions of types to and from strings.
#include <boost/lexical_cast.hpp>

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

// For debugging, this allows us to alternatively skip the MD5 checksum.
#define USE_MD5_AS_HASH 1

// If no SSL support is available, then save the opcode vector into the database
// (by skipping the generation of an MD5 checksum of the vector).
#if ( (USE_ROSE_SSL_SUPPORT == 0) && (USE_MD5_AS_HASH != 0) )
   #undef USE_MD5_AS_HASH
   #define USE_MD5_AS_HASH 0
#endif

using namespace std;
using namespace sqlite3x;
using namespace LibraryIdentification;

/**
 *  FunctionIdentification
 *
 *  Constructor.  Either opens or creates a database named dbName.
 *  Then it creates a table, vectors, if it doesn't already exist.
 *
 * @param[in] dbName  Name of the database to open
 **/
FunctionIdentification::FunctionIdentification(std::string dbName)
{
  database_name = dbName;
  //open the database
  con.open(database_name.c_str());
  con.setbusytimeout(1800 * 1000); // 30 minutes
  createTables();  
};

/**
 * @brief Create the function ID database table "vectors" if it doesn't already exist
 **/
void 
FunctionIdentification::createTables()
{
    try { //Functions: identifying hash, name, link to the source library
    con.executenonquery("create table IF NOT EXISTS functions(functionID TEXT PRIMARY KEY, function_name TEXT, libraryId TEXT)");
    //libraries: id hash, name, version, ISA, and time as an int
    //May also want calling convention and compile flags
    con.executenonquery("create table IF NOT EXISTS libraries(libraryId TEXT PRIMARY KEY, library_name TEXT, library_version TEXT, architecture INT, time INT)");
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


/**
 *  set_function_match
 *
 *  Add a function to the database.  Takes handle and opcode string.
 *  ( TODO: Use combinatorics included in rose rather than MD5. )
 *
 * @param[in] handle  Handle to the database
 * @param[in] str  A C array of characters containing the opcode
 * array, (not null terminated)
 * @param[in] str_length  The length of the opcode array
 **/
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

/**
 *  set_function_match
 *
 *  Add a function to the database.  Simple overloaded wrapper.
 *
 * @param[in] handle  Handle to the database
 * @param[in] functionString opcode string as a C++ char string
 **/
void 
FunctionIdentification::set_function_match( const library_handle & handle, const std::string functionString  )
   {
     set_function_match( handle, (const unsigned char*)functionString.c_str() , functionString.size() );
   }

/**
 *  set_function_match
 *
 *  Add a function to the database.  Simple overloaded wrapper.
 *
 * @param[in] handle  Handle to the database
 * @param[in] functionString opcode string as SgUnsignedCharList
 **/
void 
FunctionIdentification::set_function_match( const library_handle & handle, const SgUnsignedCharList & opcode_vector )
   {
     set_function_match( handle, (const unsigned char*) &(*(opcode_vector.begin())) , opcode_vector.size() );
   }



//Return the library_handle matching string from the database. bool false
//is returned if no such match was found, true otherwise.
/**
 *  get_function_match
 *
 *  Attempt to find a function in the database.  Takes reference to
 *  handle and the opcode string of the function to match.
 *  If the function is found, true is returned, and the data in the
 *  handle is filled in.  
 *  (TODO: What's in the handle if the function is not found?
 *  ( TODO: Use combinatorics included in rose rather than MD5. )
 *
 * @param[out] handle  Handle to the database
 * @param[in] str  A C array of characters containing the opcode
 * array, (not null terminated)
 * @param[in] str_length  The length of the opcode array
 * @return true if function is in database, false otherwise
 **/
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

/**
 *  get_function_match
 *
 *  Find a function in the database.  Simple overloaded wrapper.
 *
 * @param[out] handle  Handle to the database
 * @param[in] functionString opcode string as a C++ char string
 * @return true if function is in database, false otherwise
 **/
bool 
FunctionIdentification::get_function_match( library_handle & handle, const std::string functionString) const
   {
     return const_cast<FunctionIdentification*>(this)->get_function_match( handle, (const unsigned char*)functionString.c_str() , functionString.size() );
   }

/**
 *  get_function_match
 *
 *  Find a function in the database.  Simple overloaded wrapper.
 *
 * @param[out] handle  Handle to the database
 * @param[in] functionString opcode string as SgUnsignedCharList
 * @return true if function is in database, false otherwise
 **/
bool
FunctionIdentification::get_function_match( library_handle & handle, const SgUnsignedCharList & opcode_vector ) const
   {
     return const_cast<FunctionIdentification*>(this)->get_function_match( handle, (const unsigned char*) &(*(opcode_vector.begin())) , opcode_vector.size() );
   }

#endif
