// Support for writing a FLIRT data base from an existing AST.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"                                 // every librose .C file must start with this
#include "libraryIdentification.h"

#include <Rose/BinaryAnalysis/Demangler.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/GraphViz.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesM68k.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPe.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>

using namespace std;
using namespace Rose;

using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics;

namespace P2 = Partitioner2;

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

#endif
