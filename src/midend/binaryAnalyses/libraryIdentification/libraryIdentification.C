
// Support for writing a FLIRT data base from an existing AST.

#include "sage3basic.h"                                 // every librose .C file must start with this

#include <libraryIdentification.h>

using namespace std;
using namespace Rose;



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
                                                              SgProject* project,
                                                              bool replace)
{
    // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
    TimingPerformance timer ("AST Library Identification reader : time (sec) = ",true);
    
    printf ("Building LibraryIdentification database: %s from AST of library: %s : %s \n", databaseName.c_str(),libraryName.c_str(), libraryVersion.c_str());
    
    FunctionIdDatabaseInterface ident(databaseName);
    
    Rose_STL_Container<SgAsmGenericFile*> fileList = SageInterface::querySubTree<SgAsmGenericFile>(project);
    if(fileList.size() > 1) {
        std::cout << "libraryIdentification ERROR, only one library at a time" << std::endl;
        ASSERT_require(false);
    }
    LibraryInfo libraryInfo( libraryName, libraryVersion, fileList[0]);
    ident.addLibraryToDB(libraryInfo, replace);
    
    Rose_STL_Container<SgNode*> binaryInterpretationList = NodeQuery::querySubTree (project,V_SgAsmInterpretation);
    
    for (Rose_STL_Container<SgNode*>::iterator j = binaryInterpretationList.begin(); j != binaryInterpretationList.end(); j++)
        {
            SgAsmInterpretation* asmInterpretation = isSgAsmInterpretation(*j);
            ASSERT_require(asmInterpretation != NULL);
            
            //Now get all the function for the interpretation
            Rose_STL_Container<SgNode*> binaryFunctionList = NodeQuery::querySubTree (asmInterpretation,V_SgAsmFunction);
            
            for (Rose_STL_Container<SgNode*>::iterator i = binaryFunctionList.begin(); i != binaryFunctionList.end(); i++)
                {
                    // Build a pointer to the current type so that we can call the get_name() member function.
                    SgAsmFunction* binaryFunction = isSgAsmFunction(*i);
                    ROSE_ASSERT(binaryFunction != NULL);
                    
                    FunctionInfo functionInfo(binaryFunction, libraryInfo);
                    
                    
                    string mangledFunctionName   = binaryFunction->get_name();
                    string demangledFunctionName = StringUtility::demangledName(mangledFunctionName);
                    printf ("Function %s demangled = %s going into database\n", mangledFunctionName.c_str(), demangledFunctionName.c_str());
                    
                    ident.addFunctionToDB(functionInfo, replace);         
                }
        }    
}

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
LibraryIdentification::LibToFuncsMap 
LibraryIdentification::matchLibraryIdentificationDataBase (const std::string& databaseName,
                                                           SgProject* project)
{
    // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
    TimingPerformance timer ("AST Library Identification matcher : time (sec) = ",true);
    LibraryIdentification::LibToFuncsMap libToFuncsMap;
    
    FunctionIdDatabaseInterface ident(databaseName);
    
    Rose_STL_Container<SgNode*> binaryInterpretationList = NodeQuery::querySubTree (project,V_SgAsmInterpretation);
    
    for (Rose_STL_Container<SgNode*>::iterator j = binaryInterpretationList.begin(); j != binaryInterpretationList.end(); j++)
        {
            SgAsmInterpretation* asmInterpretation = isSgAsmInterpretation(*j);
            ASSERT_require(asmInterpretation != NULL);
            
            //Now get all the function for the interpretation
            Rose_STL_Container<SgNode*> binaryFunctionList = NodeQuery::querySubTree (asmInterpretation,V_SgAsmFunction);
            
            for (Rose_STL_Container<SgNode*>::iterator i = binaryFunctionList.begin(); i != binaryFunctionList.end(); i++)
                {
                    // Build a pointer to the current type so that we can call the get_name() member function.
                    SgAsmFunction* binaryFunction = isSgAsmFunction(*i);
                    ROSE_ASSERT(binaryFunction != NULL);
                    
                    FunctionInfo functionInfo(binaryFunction);
                    
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
        }    
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


