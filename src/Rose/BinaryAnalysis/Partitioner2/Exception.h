#ifndef ROSE_BinaryAnalysis_Partitioner2_Exception_H
#define ROSE_BinaryAnalysis_Partitioner2_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/Exception.h>

#include <stdexcept>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

class Exception: public Rose::Exception {
public:
    Exception(const std::string &mesg): Rose::Exception(mesg) {}
    ~Exception() throw() {}
};

class PlaceholderError: public Exception {
    Address startVa_;
public:
    PlaceholderError(Address startVa, const std::string &mesg)
        : Exception(mesg), startVa_(startVa) {}
    ~PlaceholderError() throw() {}
    Address startVa() const { return startVa_; }
};

class BasicBlockError: public Exception {
    BasicBlockPtr bblock_;
public:
    BasicBlockError(const BasicBlockPtr&, const std::string &mesg);
    ~BasicBlockError() throw();
    BasicBlockPtr bblock() const;
};

class DataBlockError: public Exception {
    DataBlockPtr dblock_;
public:
    DataBlockError(const DataBlockPtr&, const std::string &mesg);
    ~DataBlockError() throw();
    DataBlockPtr dblock() const;
};

class FunctionError: public Exception {
    FunctionPtr function_;
public:
    FunctionError(const FunctionPtr&, const std::string &mesg);
    ~FunctionError() throw();
    FunctionPtr function() const;
};

class FileError: public Exception {
public:
    FileError(const std::string &mesg)
        : Exception(mesg) {}
    ~FileError() throw() {}
};

} // namespace
} // namespace
} // namespace

#endif
#endif
