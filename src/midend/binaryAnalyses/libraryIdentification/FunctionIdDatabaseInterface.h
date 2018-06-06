#ifndef FUNCTION_ID_DATABASE_INTERFACE_H
#define FUNCTION_ID_DATABASE_INTERFACE_H

#include "sqlite3x.h"
#include "LibraryInfo.h"
#include "FunctionInfo.h"


/** LibraryIdentification.
 *
 *  This namespace encapsulates function for FLIRT ( Fast Library
 *  Identification and Recognition Technology) like functionality for
 *  ROSE binary analysis.
 **/
namespace LibraryIdentification
{

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
        FunctionIdDatabaseInterface(std::string dbName);
        

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
         void addFunctionToDB(const FunctionInfo& fInfo, bool replace = false);

         /** @brief Lookup a function in the database.  True returned if found
          *  @param[inout] fInfo The FunctionInfo only needs to
          *  contain the hash, the rest will be filled in.
          **/
         bool matchFunction(FunctionInfo& fInfo);
         

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
     sqlite3x::sqlite3_connection con;
    };   
}


#endif

