
// Support for writing a FLIRT data base from an existing AST.

#include "sage3basic.h"                                 // every librose .C file must start with this

#include <libraryIdentification.h>
#include <BinaryDemangler.h>
#include <Partitioner2/Engine.h>
#include <Partitioner2/GraphViz.h>
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/Modules.h>
#include <Partitioner2/Utility.h>

using namespace std;
using namespace Rose;

using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics;

namespace P2 = Partitioner2;


/**  generateLibraryIdentificationDataBase
 *  
 *  Creates or updates a binary function identification database with
 *  the functions in the project.  
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
void
LibraryIdentification::generateLibraryIdentificationDataBase( const std::string& databaseName, 
                                                              const std::string& libraryName, 
                                                              const std::string& libraryVersion, 
                                                              const std::string& libraryHash, 
                                                              const P2::Partitioner& partitioner,
                                                              enum DUPLICATE_OPTION dupOption)
{
    Rose::BinaryAnalysis::Demangler demangler;
    // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
    TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);
    
    mlog[INFO] << "Building LibraryIdentification database: " << databaseName.c_str() << " from AST of Library: " << libraryName.c_str() << " : " << libraryVersion.c_str() << std::endl;
    
    FunctionIdDatabaseInterface ident(databaseName);

    const std::string& libraryIsa = partitioner.instructionProvider().disassembler()->name();

    LibraryInfo libraryInfo( libraryName, libraryVersion, libraryHash, libraryIsa); 
    ident.addLibraryToDB(libraryInfo, dupOption == NO_ADD ? false : true); //libraries don't have a "COMBINE option, so convert dupOption to a bool replace
    
    //Now get all the functions in the library
    std::vector< P2::Function::Ptr > binaryFunctionList = partitioner.functions();
    
    for (std::vector< P2::Function::Ptr >::iterator funcIt = binaryFunctionList.begin(); funcIt != binaryFunctionList.end(); funcIt++)
        {
            // Build a pointer to the current type so that we can call the get_name() member function.
            P2::Function::Ptr binaryFunction = *funcIt;
            ROSE_ASSERT(binaryFunction != NULL);
            
            FunctionInfo functionInfo(partitioner, binaryFunction, libraryInfo);
            
            string mangledFunctionName = binaryFunction->name();
            string demangledFunctionName = binaryFunction->demangledName();
            mlog[INFO] << "Function " << mangledFunctionName.c_str() << " demangled = " << demangledFunctionName.c_str() << "going into database" << std::endl;           
            
            ident.addFunctionToDB(functionInfo, dupOption);         
        }
            
}

/** match functions against the Library Identification Database
 *  This is a function to simplify matching functions in a binary
 *  partitioner to library functions in the database.  It will attempt to
 *  match every function defined in the root to a library function.
 *
 *  It returns a LibToFuncsMap that contains every function defined in
 *  the project in the following form: Library->set(Function).  
 *  Functions that could not be matched in the database are found in
 *  the "UNKNOWN" library.
 *  Functions that cannot be *uniquely* identified, end up in the
 *  "multiLibarary" special library.
 *
 * @param[in] databaseName Filename of the database to create/access
 * @param[in] partitioner  Binary partitioner has the functions to
 * write or find
 * @return libToFuncsMap Libraries->set(Functions) unmatched
 * functions under "UNKNOWN", multimatched functions returned in 
 * "MULTIPLE_LIBS"
 **/
LibraryIdentification::LibToFuncsMap 
LibraryIdentification::matchLibraryIdentificationDataBase (const std::string& databaseName,
                                                           const P2::Partitioner& partitioner)
{
    // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
    TimingPerformance timer ("AST Library Identification matcher : time (sec) = ",true);
    LibraryIdentification::LibToFuncsMap libToFuncsMap;
    
    FunctionIdDatabaseInterface ident(databaseName);            
    std::vector< P2::Function::Ptr > binaryFunctionList = partitioner.functions();

    for (std::vector< P2::Function::Ptr >::iterator funcIt = binaryFunctionList.begin(); funcIt != binaryFunctionList.end(); funcIt++)
        {            
            P2::Function::Ptr binaryFunction = *funcIt;
            ROSE_ASSERT(binaryFunction != NULL);
            FunctionInfo functionInfo(partitioner, binaryFunction);
            
            std::vector<FunctionInfo> matches = ident.matchFunction(functionInfo);
            
            if(matches.size() == 0) 
                {
                    //No match, leave name from binary and put it under the UNKNOWN library
                    LibraryInfo libraryInfo = LibraryInfo::getUnknownLibraryInfo();
                    functionInfo.libHash = libraryInfo.libHash;
                    insertFunctionToMap(libToFuncsMap, libraryInfo, functionInfo);
                } 

            else if(matches.size() > 1) 
                { //Lots of matches, stick in the multi-library.
                  //Use the name in the binary if it exists, otherwise
                  //first match  
                    functionInfo = matches[0];
                    if(!binaryFunction->name().empty()) 
                        {
                            functionInfo.funcName = binaryFunction->name();                    
                        } 

                    LibraryInfo libraryInfo = LibraryInfo::getMultiLibraryInfo();
                    ident.matchLibrary(libraryInfo);
                    insertFunctionToMap(libToFuncsMap, libraryInfo, functionInfo);
                } 
            else  //Only one match, positive ID!
                {                              
                    functionInfo = matches[0];
                    LibraryInfo libraryInfo(functionInfo.libHash);
                    ident.matchLibrary(libraryInfo);
                    insertFunctionToMap(libToFuncsMap, libraryInfo, functionInfo);
                }
        }

    return libToFuncsMap;
}

/** insertFunctionToMap
 *
 *  Helper function, when a matching function is found in the
 *  database, it needs to be added to the map returned from
 *  matchLibraryIdentificationDataBase.  This function does that.
 *
 * @param[inout] libToFuncsMap Lists every library, and all the
 *                             functions is contains.  Insert function
 *                             to this data structure.
 * @param[in] libraryInfo      Fully describes the library to add(?)
 * @param[in] functionInfo     Fully describes the function to add
 **/
void LibraryIdentification::insertFunctionToMap(LibraryIdentification::LibToFuncsMap& libToFuncsMap, const LibraryInfo& libraryInfo, const FunctionInfo& functionInfo) 
{
    LibraryIdentification::LibToFuncsMap::iterator libIt = libToFuncsMap.find(libraryInfo);
    if(libIt == libToFuncsMap.end()) //If no library was found, add it.
        {
            std::set<FunctionInfo> funcSet;
            funcSet.insert(functionInfo);
            std::pair<LibraryInfo, std::set<FunctionInfo> > insertPair(libraryInfo, funcSet);
            libToFuncsMap.insert(insertPair);
        } else 
        {
            std::set<FunctionInfo>& funcSet = libIt->second;
            funcSet.insert(functionInfo);
        }
}


