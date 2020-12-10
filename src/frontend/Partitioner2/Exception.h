#ifndef ROSE_Partitioner2_Exception_H
#define ROSE_Partitioner2_Exception_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>
#include <RoseException.h>

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
    rose_addr_t startVa_;
public:
    PlaceholderError(rose_addr_t startVa, const std::string &mesg)
        : Exception(mesg), startVa_(startVa) {}
    ~PlaceholderError() throw() {}
    rose_addr_t startVa() const { return startVa_; }
};

class BasicBlockError: public Exception {
    BasicBlock::Ptr bblock_;
public:
    BasicBlockError(const BasicBlock::Ptr &bblock, const std::string &mesg)
        : Exception(mesg), bblock_(bblock) {}
    ~BasicBlockError() throw() {}
    BasicBlock::Ptr bblock() const { return bblock_; }
};

class DataBlockError: public Exception {
    DataBlock::Ptr dblock_;
public:
    DataBlockError(const DataBlock::Ptr &dblock, const std::string &mesg)
        : Exception(mesg), dblock_(dblock) {}
    ~DataBlockError() throw() {}
    DataBlock::Ptr dblock() const { return dblock_; }
};

class FunctionError: public Exception {
    Function::Ptr function_;
public:
    FunctionError(const Function::Ptr &function, const std::string &mesg)
        : Exception(mesg), function_(function) {}
    ~FunctionError() throw() {}
    Function::Ptr function() const { return function_; }
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
