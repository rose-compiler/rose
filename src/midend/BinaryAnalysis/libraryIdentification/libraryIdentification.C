// Support for writing a FLIRT data base from an existing AST.
#include <featureTests.h>
#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION
#include <sage3basic.h>
#include "libraryIdentification.h"

#include <Rose/BinaryAnalysis/LibraryIdentification.h>

using namespace Rose::BinaryAnalysis;
namespace P2 = Partitioner2;
using Flir = Rose::BinaryAnalysis::LibraryIdentification;

// Deprecated [Robb Matzke 2022-01-20]: Incorrect namespace "::LibraryIdentification"
namespace LibraryIdentification {

void
generateLibraryIdentificationDataBase(const std::string &databaseName, const std::string &libraryName,
                                      const std::string &libraryVersion, const std::string &libraryHash,
                                      const P2::Partitioner &partitioner, enum DUPLICATE_OPTION dupOption) {
    ASSERT_require(COMBINE == dupOption);

    Flir flir;
    try {
        flir.connect("sqlite://" + databaseName);
    } catch (...) {
        flir.createDatabase("sqlite://" + databaseName);
    }

    const std::string arch = partitioner.instructionProvider().disassembler()->name();
    auto lib = Flir::Library::instance(libraryHash, libraryName, libraryVersion, arch);
    flir.insertLibrary(lib, partitioner);
}

LibToFuncsMap
matchLibraryIdentificationDataBase(const std::string &databaseName, const P2::Partitioner &partitioner) {
    LibToFuncsMap retval;
    Flir flir;
    flir.connect("sqlite://" + databaseName);
    for (const P2::Function::Ptr &function: partitioner.functions()) {
        std::vector<Flir::Function::Ptr> matches = flir.search(partitioner, function);
        FunctionInfo functionInfo(partitioner, function);
        if (matches.empty()) {
            // No match, leave name from binary and put it under the UNKNOWN library
            auto libraryInfo = LibraryInfo::getUnknownLibraryInfo();
            functionInfo.libHash = libraryInfo.libHash;
            insertFunctionToMap(retval, libraryInfo, functionInfo);

        } else {
            const bool isMulti = matches.size() > 1;
            auto libraryInfo = isMulti ? LibraryInfo::getMultiLibraryInfo() : LibraryInfo(matches[0]->library()->hash());
            libraryInfo.libName = matches[0]->library()->name();
            libraryInfo.libHash = matches[0]->library()->hash();
            libraryInfo.libVersion = matches[0]->library()->version();
            libraryInfo.architecture = matches[0]->library()->architecture();
            libraryInfo.analysisTime = 0;

            // If lots of matches, stick in the multi-library. Use the name in the binary if it exists, otherwise first
            // match. Otherwise if one match, positive ID!
            functionInfo.libHash = libraryInfo.libHash;
            if (isMulti) {
                functionInfo.funcName = function->name();
            } else {
                functionInfo.funcName = matches[0]->name();
                functionInfo.funcHash = matches[0]->hash();
            }

            insertFunctionToMap(retval, libraryInfo, functionInfo);
        }
    }
    return retval;
}

void
insertFunctionToMap(LibToFuncsMap &libToFuncsMap, const LibraryInfo &libraryInfo, const FunctionInfo &functionInfo) {
    LibToFuncsMap::iterator libIt = libToFuncsMap.find(libraryInfo);
    if (libToFuncsMap.end() == libIt) {
        std::set<FunctionInfo> funcSet;
        funcSet.insert(functionInfo);
        std::pair<LibraryInfo, std::set<FunctionInfo>> insertPair(libraryInfo, funcSet);
        libToFuncsMap.insert(insertPair);
    } else {
        std::set<FunctionInfo>& funcSet = libIt->second;
        funcSet.insert(functionInfo);
    }
}

} // namespace

#endif
