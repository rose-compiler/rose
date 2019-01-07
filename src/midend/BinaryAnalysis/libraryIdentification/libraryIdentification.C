
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
 * @param[in] project      Rose SgProject that has the functions to
 * write or find
 **/
void
LibraryIdentification::generateLibraryIdentificationDataBase( const std::string& databaseName, 
                                                              const std::string& libraryName, 
                                                              const std::string& libraryVersion, 
                                                              const std::string& libraryHash, 
                                                              const P2::Partitioner& partitioner,
                                                              bool replace)
{
    Rose::BinaryAnalysis::Demangler demangler;
    // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
    TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);
    
    //TODO Switch these printfs to mlog
    printf ("Building LibraryIdentification database: %s from AST of library: %s : %s \n", databaseName.c_str(),libraryName.c_str(), libraryVersion.c_str());
    
    FunctionIdDatabaseInterface ident(databaseName);

    const std::string& libraryIsa = partitioner.instructionProvider().disassembler()->name();

    LibraryInfo libraryInfo( libraryName, libraryVersion, libraryHash, libraryIsa); 
    ident.addLibraryToDB(libraryInfo, replace);
    
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
            printf ("Function %s demangled = %s going into database\n", mangledFunctionName.c_str(), demangledFunctionName.c_str());
            
            ident.addFunctionToDB(functionInfo, replace);         
        }
            
}

/** match functions in root to  Library Identification Database
 *  This is a function to simplify matching functions in a binary
 *  root to library functions in the database.  It will attempt to
 *  match every function defined in the root to a library function.
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
LibraryIdentification::LibToFuncsMap 
LibraryIdentification::matchLibraryIdentificationDataBase (const std::string& databaseName,
                                                           const P2::Partitioner& partitioner)
{
    // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
    TimingPerformance timer ("AST Library Identification matcher : time (sec) = ",true);
    LibraryIdentification::LibToFuncsMap libToFuncsMap;
    
    FunctionIdDatabaseInterface ident(databaseName);
    //    Rose_STL_Container<SgNode*> binaryFunctionList = NodeQuery::querySubTree (root,V_SgAsmFunction);
//     Rose_STL_Container<SgNode*> binaryInterpretationList = NodeQuery::querySubTree (root,V_SgAsmInterpretation);
    
//     std::cerr << "Queried for Interpretations " << std::endl;
    

//     for (Rose_STL_Container<SgNode*>::iterator j = binaryInterpretationList.begin(); j != binaryInterpretationList.end(); j++)
//         {
//             std::cerr << "Interpretation " << (size_t)(j - binaryInterpretationList.begin()) << std::endl;
//             SgAsmInterpretation* asmInterpretation = isSgAsmInterpretation(*j);
//             ASSERT_require(asmInterpretation != NULL);
            
//             //Now get all the function for the interpretation
//             Rose_STL_Container<SgNode*> binaryFunctionList = NodeQuery::querySubTree (asmInterpretation,V_SgAsmFunction);
//
//    for (Rose_STL_Container<SgNode*>::iterator i = binaryFunctionList.begin(); i != binaryFunctionList.end(); i++)
        
            
    std::vector< P2::Function::Ptr > binaryFunctionList = partitioner.functions();
    for (std::vector< P2::Function::Ptr >::iterator funcIt = binaryFunctionList.begin(); funcIt != binaryFunctionList.end(); funcIt++)
        {            
            P2::Function::Ptr binaryFunction = *funcIt;
            ROSE_ASSERT(binaryFunction != NULL);
            FunctionInfo functionInfo(partitioner, binaryFunction);
            
            //If the library did not have a name for this function,
            //might the current binary?
            if(functionInfo.funcName.empty() &&
               !binaryFunction->name().empty()) 
                {  
                    functionInfo.funcName = binaryFunction->name();                    
                }
            
            if(ident.matchFunction(functionInfo)) 
                { //match, insert it into the libToFuncsMap
                    LibraryInfo libraryInfo(functionInfo.libHash);
                    ident.matchLibrary(libraryInfo);
                    insertFunctionToMap(libToFuncsMap, libraryInfo, functionInfo);
                } 
            else 
                {  //No match, put it under the UNKNOWN library
                    LibraryInfo libraryInfo = LibraryInfo::getUnknownLibraryInfo();
                    functionInfo.libHash = libraryInfo.libHash;
                    insertFunctionToMap(libToFuncsMap, libraryInfo, functionInfo);                            
                }
        }
    //}    
    return libToFuncsMap;
}

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


