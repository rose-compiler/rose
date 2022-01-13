#ifndef FUNCTION_ID_DATABASE_INTERFACE_H
#define FUNCTION_ID_DATABASE_INTERFACE_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include "sqlite3x.h"
#include "LibraryInfo.h"
#include "FunctionInfo.h"
#include <vector>
#include <ctype.h>

// documented elsewhere
namespace LibraryIdentification
{
    /** Option for what to do if a function with the same hash is found when adding a function. */
    enum DUPLICATE_OPTION
    {
        UNKNOWN,
        COMBINE,        /**< Allow both hashes to exist. */
        REPLACE,        /**< Replace the old function with this new function. This will eliminate ALL old functions with the
                         *   same hash. */
        NO_ADD          /**< Do not add the function, leave the old functions in the database. */
    };

    /** Parse enum constant from string. */
    enum DUPLICATE_OPTION duplicateOptionFromString(std::string option);

    /** Database interface for storing library function signatures.
     *
     *  Creates and holds the handle for the SQLite database used for storing and matching libraries and functions, FLIRT
     *  style.
     **/
    class FunctionIdDatabaseInterface 
    {
    public:
        /** Constructor.
         *
         *  Either opens or creates a database with file named dbName.  The database should have two tables in it, functions
         *  and libraries.
         *
         * @param[in] dbName  Name of the database to open
         **/
        FunctionIdDatabaseInterface(const std::string& dbName);

         /** Ensures that tables exist in the database.
          *
          * Makes sure that all the tables are defined in the function identification database.  The database should have two
          * tables in it, functions and libraries.  Functions uniquely identifies functions and the library they belong to.
          * Libraries uniquely identifies libraries, and contains metadata about them.
          **/
        void createTables();
 
         /** Add an entry for a function to the database. */
         void addFunctionToDB(const FunctionInfo& fInfo, enum DUPLICATE_OPTION dupOption = COMBINE);

         /** Lookup a single function in the database.
          *
          *  Returns true if found, false otherwise.
          *
          *  @param[inout] fInfo The FunctionInfo only needs to contain the hash, the rest will be filled in.
          *  @return: True if a function was found
          **/
         bool matchOneFunction(FunctionInfo& fInfo);
         
         /** Lookup all functions with the hash in the database.
          *
          *  @param[inout] funcInfo is used as a prototype for all the returned FunctionInfos
          *  @return A vector of all found functions
          **/
         std::vector<FunctionInfo> matchFunction(const FunctionInfo& fInfo);

         /** Exactly lookup a function in the database.
          *
          *  There should only be one that matches ID, name, and library hash.
          *
          *  @param[inout] fInfo The FunctionInfo only needs to contain the hash, name, and library hash. There should only be
          *  one matching function in the database.
          *  @return true if found
          **/
         bool exactMatchFunction(const FunctionInfo& fInfo);
         
         /** Removes any functions that match the hash from the database.
          *
          *  @param[inout] The hash to remove from the database
          **/
         void removeFunctions(const std::string& funcHash);
         

         /** Add an entry for a library to the database. */
         void addLibraryToDB(const LibraryInfo& lInfo, bool replace = false);

         /** Lookup a library in the database.
          *
          *  Returns true if found, false otherwise.
          *
          *  @param[inout] lInfo The LibraryInfo only needs to contain the hash, the rest will be filled in.
          **/
         bool matchLibrary(LibraryInfo& fInfo);

 private:
     // The name of the database
     std::string database_name;
     
     // SQLite database handle
     sqlite3x::sqlite3_connection sqConnection;
    };
}

#endif
#endif
