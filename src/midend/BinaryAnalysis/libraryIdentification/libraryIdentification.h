#ifndef LIBRARY_IDENTIFICATION_H
#define LIBRARY_IDENTIFICATION_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "LibraryInfo.h"
#include "FunctionInfo.h"
#include "FunctionIdDatabaseInterface.h"

/** LibraryIdentification.
 *
 *  This namespace encapsulates function for FLIRT ( Fast Library
 *  Identification and Recognition Technology) like functionality for
 *  ROSE binary analysis.
 **/
namespace LibraryIdentification
   {

/** typedef libToFuncMap
 *  This is used as the type to list which functions are found in
 *  which libraries from matchLibraryIdentificationDataBase.
 *  The map is: libraryname -> set<functions matched in that library>
 *
 *  Functions that are not found in any library, will be placed in the
 *  "UNKNOWN" bin.
 **/
       typedef std::map<LibraryInfo, std::set<FunctionInfo> > LibToFuncsMap;
       

/** generate Library Identification Database
 *  This function takes a binary project (presumeably a library) and
 *  hashes every function, in it.  It then inserts the library and
 *  functions into a new sqlite3 database.  If the project was
 *  built with debug information, we should have a database that can
 *  later identify functions in stripped libraries.
 *
 * @param[in] databaseName Filename of the database to create/access
 * @param[in] libraryName  Library names cannot be discovered from all
 *                         library types, so pass in name.
 * @param[in] libraryVersion  Library version, same problem
 * @param[in] libraryHash  Unique hash identifing the libary.
 *                         Partitioner can't generate it
 * @param[in] partitioner  The main ROSE binary anlysis object,
 *                         contains all functions, code, etc.
 * @param[in] dupOption    tells what to do with duplicate functions
 **/     
       void generateLibraryIdentificationDataBase    ( const std::string& databaseName, 
                                                       const std::string& libraryName, 
                                                       const std::string& libraryVersion, 
                                                       const std::string& libraryHash, 
                                                       const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner,
                                                       enum DUPLICATE_OPTION dupOption = COMBINE);



/** match functions in project to  Library Identification Database
 *  This is a function to simplify matching functions in a binary
 *  project to library functions in the database.  It will attempt to
 *  match every function defined in the project to a library function.
 *
 *  It returns a LibToFuncsMap that contains every function defined in
 *  the project in the following form: Library->set(Function).  
 *  Functions that could not be matched in the database are found in
 *  the "UNKNOWN" library.
 *
 * @param[in] databaseName Filename of the database to create/access
 * @param[in] partitioner  Binary partitioner has the functions to
 * write or find
 * @return libToFuncsMap Libraries->set(Functions) unmatched
 * functions under "UNKNOWN", multimatched functions returned in
 * "MULTIPLE_LIBS" 
 **/     
       LibToFuncsMap matchLibraryIdentificationDataBase (const std::string& databaseName,
                                                         const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner);

/** 
 * Private helper function for adding idents to the libToFuncsMap.
 * @param[inout] libToFuncsMap The map to insert to
 * @param[in] libraryInfo      This libraryInfo to insert as key
 * @param[in] functionInfo     The functionInfo to insert as value        
 **/     
       void insertFunctionToMap(LibToFuncsMap& libToFuncsMap, 
                                const LibraryInfo& libraryInfo, 
                                const FunctionInfo& functionInfo);
       
       

   }

#endif
#endif
