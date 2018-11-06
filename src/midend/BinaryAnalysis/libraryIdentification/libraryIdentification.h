#ifndef LIBRARY_IDENTIFICATION_H
#define LIBRARY_IDENTIFICATION_H

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
 * @param[in] project      Rose SgProject that has the functions to
 * write or find
 * @param[in] replace      If a function or library already exisits in
 * the database, replace it?
 **/     
       void generateLibraryIdentificationDataBase    ( const std::string& databaseName, 
                                                       const std::string& libraryName, 
                                                       const std::string& libraryVersion, 
                                                       SgNode* root,
                                                       bool replace = false);



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
 * @param[in] project      Rose SgProject that has the functions to
 * write or find
 * @return libToFuncsMap Libraries->set(Functions) unmatched
 * functions under "UNKNOWN"
 **/     
       LibToFuncsMap matchLibraryIdentificationDataBase (const std::string& databaseName,
                                                          SgNode* root);

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
