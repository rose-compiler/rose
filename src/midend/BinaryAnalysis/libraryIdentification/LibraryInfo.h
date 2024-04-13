#ifndef LIBRARY_INFO_H
#define LIBRARY_INFO_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION

#include <Rose/BinaryAnalysis/AstHasher.h>
#include <time.h>
#include "Combinatorics.h"

// Deprecated [Robb Matzke 2022-01-20]
namespace LibraryIdentification {

const std::string unknownLibraryName = "UNKNOWN";
const std::string multiLibraryName = "MULTIPLE_LIBS";

// Deprecated [Robb Matzke 2022-01-20]
class LibraryInfo {
public:
    // Constructor.
    //
    // Combines all the information required to identify a library.  This constructor allows the user to define
    // everything.
    //
    // @param[in] libName  Name of the library to add.
    // @param[in] libVersion  Version of the library. Please use Fowler-Noll-Vo HasherFnv class in Combinatorics.h.
    // @param[in] libHash  Unique Hash of the library to add. Please use Fowler-Noll-Vo HasherFnv class in Combinatorics.h.
    // @param[in] architecture  architecture library was built for.
    LibraryInfo(const std::string& libName, const std::string& libVersion, const std::string& libHash,
                const std::string& architecture)
        : libName(libName), libVersion(libVersion), libHash(libHash), architecture(architecture) {
            analysisTime = time(NULL);
    }

    // Constructor.
    //
    // Only takes the hash, Rest to be filled in from matching in the database.
    //
    // @param[in] libHash  Unique Hash of the library to add
    LibraryInfo(const std::string& libHash)
        : libName(unknownLibraryName), libVersion(unknownLibraryName), libHash(libHash), architecture(unknownLibraryName) {}

    // True if the hashes are in ascending order.
    //
    // Lessthan operator for sorting and recognizing duplicates.
    friend bool operator<(const LibraryInfo& lhs,const LibraryInfo& rhs) {
        return lhs.libHash < rhs.libHash;
    }
        
    // Factory for "Unknown Library" instance.
    //
    // Constructs and returns the special sentinal "Unknown Library" instance for functions that couldn't be identified.
    static LibraryInfo getUnknownLibraryInfo() {
        return LibraryInfo(unknownLibraryName, unknownLibraryName, unknownLibraryName,  unknownLibraryName);
    }

    // Factory for "Multi Library" instance.
    //
    // Constructs and returns the special sentinal "Multi Library" instance for functions that couldn't be uniquely
    // identified. ie, a function with this hash appears in multiple libraries (probably with multiple names).
    static LibraryInfo getMultiLibraryInfo() {
        return LibraryInfo(multiLibraryName, multiLibraryName, multiLibraryName,  multiLibraryName);
    }

    // Human readable libary identifier
    std::string toString() {
        return libName + "." + libVersion;
    }

    // The name of the library.
    std::string libName;

    // The version of the library.
    std::string libVersion;

    // A hash that should uniquely identify the library.
    std::string libHash;

    // Instruction set for which library was built.
    std::string architecture;

    // The time when this library was processed.
    time_t analysisTime;
};

} // namespace

#endif
#endif
