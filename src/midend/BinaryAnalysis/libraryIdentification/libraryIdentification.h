#ifndef LIBRARY_IDENTIFICATION_H
#define LIBRARY_IDENTIFICATION_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include "LibraryInfo.h"
#include "FunctionInfo.h"
#include "FunctionIdDatabaseInterface.h"

/** Identification and recognition of library functions.
 *
 *  This namespace encapsulates functions for FLIRT-like (Fast Library Identification and Recognition Technology) functionality
 *  for ROSE binary analysis.
 **/
namespace LibraryIdentification {

/** Mapping from library to information about contained functions.
 *
 *  This is used as the type to list which functions are found in which libraries from @ref matchLibraryIdentificationDataBase.
 *
 *  Functions that are not found in any library, will be placed in the "UNKNOWN" bin.
 **/
typedef std::map<LibraryInfo, std::set<FunctionInfo> > LibToFuncsMap;
       

/** Hash library functions and insert into database.
 *
 *  This function takes a binary specimen (presumeably a library) and hashes every function, in it.  It then inserts the
 *  library and functions into a new SQLite database.  If the specimen contains debug information, the resulting database will
 *  contain information that can later identify functions in stripped libraries.
 *
 * @param[in] databaseName Filename of the database to create/access
 * @param[in] libraryName  Library names cannot be discovered from all library types, so pass in name.
 * @param[in] libraryVersion  Library version, same problem
 * @param[in] libraryHash  Unique hash identifing the libary. Partitioner can't generate it
 * @param[in] partitioner  The main ROSE binary anlysis object, contains all functions, code, etc.
 * @param[in] dupOption    tells what to do with duplicate functions
 **/     
void generateLibraryIdentificationDataBase(const std::string& databaseName,
                                           const std::string& libraryName,
                                           const std::string& libraryVersion,
                                           const std::string& libraryHash,
                                           const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner,
                                           enum DUPLICATE_OPTION dupOption = COMBINE);

/** Attempt to match specimen functions to database functions.
 *
 *  This function attempts to match functions in a binary specimen to library functions in the database.  It will attempt to
 *  match every function defined in the project to a library function.
 *
 *  It returns a LibToFuncsMap that contains every function defined in the project in the following form:
 *  Library->set(Function).  Functions that could not be matched in the database are found in the "UNKNOWN" library.
 *
 * @param[in] databaseName Filename of the database to create/access
 * @param[in] partitioner  Binary partitioner has the functions to write or find
 * @return libToFuncsMap Libraries->set(Functions) unmatched functions under "UNKNOWN", multimatched functions returned in
 * "MULTIPLE_LIBS"
 **/     
LibToFuncsMap matchLibraryIdentificationDataBase(const std::string& databaseName,
                                                 const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner);

// Private helper function for adding idents to the libToFuncsMap.
void insertFunctionToMap(LibToFuncsMap& libToFuncsMap, // the map to insert into
                         const LibraryInfo& libraryInfo, // library to insert as key
                         const FunctionInfo& functionInfo); //  function to insert as key

} // namespace

#endif
#endif
