#ifndef FUNCTION_ID_DATABASE_INTERFACE_H
#define FUNCTION_ID_DATABASE_INTERFACE_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "sqlite3x.h"
#include "LibraryInfo.h"
#include "FunctionInfo.h"
#include <vector>
#include <ctype.h>

/** LibraryIdentification.
 *
 *  This namespace encapsulates function for FLIRT ( Fast Library
 *  Identification and Recognition Technology) like functionality for
 *  ROSE binary analysis.
 **/
namespace LibraryIdentification
{
    /** @enum DUPLICATE_OPTION
     *
     *  Option for what to do if a function with the same hash is
     *  found when adding a function.
     *  COMBINE: Allow both hashes to exist
     *  REPLACE: Replace the old function with this new function.
     *    (Will eliminate ALL old functions with the same hash)
     *  NO_ADD:  Do not add the function, leave the old functions in
     *    the database
     **/
    enum DUPLICATE_OPTION 
    {
        UNKNOWN,
        COMBINE,
        REPLACE,
        NO_ADD
    };
    
    enum DUPLICATE_OPTION duplicateOptionFromString(std::string option);


/** @class FunctionIdDatabaseInterface 
 *
 *  Creates and holds the handle for the sqlite database used for
 *  storing and matching libraries and functions, FLIRT style.
 **/
    class FunctionIdDatabaseInterface 
    {
    public:
        /**
         *  FunctionIdentification
         *
         *  Constructor.  Either opens or creates a database with file
         *  named dbName.
         *  The database should have two tables in it, functions and
         *  libraries. 
         *
         * @param[in] dbName  Name of the database to open
         **/
        FunctionIdDatabaseInterface(const std::string& dbName);
        

         /** 
          * createTables 
          * Make sure that all the tables are defined in the function
          * identification database.    
          * The database should have two tables in it, functions and
          * libraries.
          * Functions uniquely identifies functions and the library
          * they belong to.
          * Libraries uniquely identifies libraries, and contains
          * metadata about them.
          **/
        void createTables();
 

         // @brief Add an entry for a function to the database
         void addFunctionToDB(const FunctionInfo& fInfo, enum DUPLICATE_OPTION dupOption = COMBINE);

         /** @brief Lookup a single function in the database. True 
          *  returned if found
          *  @param[inout] fInfo The FunctionInfo only needs to
          *  contain the hash, the rest will be filled in.
          *  @return: True if a function was found
          **/
         bool matchOneFunction(FunctionInfo& fInfo);
         
         /** @brief Lookup all functions with the hash in the database.
          *  @param[inout] funcInfo is used as a prototype for all the
          *  returned FunctionInfos
          *  @return A vector of all found functions
          **/
         std::vector<FunctionInfo> matchFunction(const FunctionInfo& fInfo);


         /** @brief Exactly lookup a function in the database.  There should
          *  only be one that matches Id, name, and library hash
          *  @param[inout] fInfo The FunctionInfo only needs to
          *  contain the hash, name, and library hash. There should only be one
          *  matching function in the database.
          *  @return true if found
          **/
         bool exactMatchFunction(const FunctionInfo& fInfo);
         

         /** @brief Removes any functions that match the hash from the
          *  database.
          *  @param[inout] The hash to remove from the database
          **/
         void removeFunctions(const std::string& funcHash);
         

         // @brief Add an entry for a library to the database
         void addLibraryToDB(const LibraryInfo& lInfo, bool replace = false);

         /** @brief Lookup a library in the database.  True returned if found
          *  @param[inout] lInfo The LibraryInfo only needs to
          *  contain the hash, the rest will be filled in.
          **/
         bool matchLibrary(LibraryInfo& fInfo);

 private:
     // @brief The name of the database
     std::string database_name;
     
     // @brief SQLite database handle
     sqlite3x::sqlite3_connection sqConnection;

    };   
}

#endif
#endif
