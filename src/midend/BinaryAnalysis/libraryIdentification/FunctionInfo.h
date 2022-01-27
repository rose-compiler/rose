#ifndef FUNCTION_INFO_H
#define FUNCTION_INFO_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_LIBRARY_IDENTIFICATION

#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>

// Deprecated [Robb Matzke 2022-01-20]
namespace LibraryIdentification {

// Deprecated [Robb Matzke 2022-01-20]
// Information about a function.
//
// Combines all the information to uniquely identify a single function in one object.
//
class FunctionInfo {
public:
    // Constructor.
    //
    // Combines all the information required to identify a function.  This constructor constructs the hash from the
    // SgAsmFunction node.  Note that currently on FNV hasher is used.  This should be an option.
    //
    // @param[in] partitioner Required to get the basic blocks of the function
    // @param[in] function Binary AST Function Node
    FunctionInfo(const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner,
                 Rose::BinaryAnalysis::Partitioner2::Function::Ptr function)
        : funcName(function->name()), binaryFunction(function) {}

    // True if the first hash is less than the second hash.
    friend bool operator<(const FunctionInfo& lhs, const FunctionInfo& rhs) {
        return lhs.funcHash < rhs.funcHash;
    }

    // True if the name, function hash, and library hash are equal.
    bool operator==(const FunctionInfo& rhs) {
        return funcName == rhs.funcName && funcHash == rhs.funcHash && libHash == rhs.libHash;
    }

    // The name of the function.
    std::string funcName;

    // A hash that should uniquely identify the function.
    std::string funcHash;

    // Hash uniquely identifying the library to which the function belongs.
    std::string libHash;

    // Optional pointer to the function definition.
    //
    // Null if the function definition is not available. */
    Rose::BinaryAnalysis::Partitioner2::Function::Ptr binaryFunction;
};

} // namespace

#endif
#endif
