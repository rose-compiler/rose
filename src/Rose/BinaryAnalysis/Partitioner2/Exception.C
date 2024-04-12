#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/Exception.h>

#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BasicBlockError
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BasicBlockError::BasicBlockError(const BasicBlock::Ptr &bblock, const std::string &mesg)
    : Exception(mesg), bblock_(bblock) {}

BasicBlockError::~BasicBlockError() throw() {}

BasicBlock::Ptr
BasicBlockError::bblock() const {
    return bblock_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DataBlockError
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DataBlockError::DataBlockError(const DataBlock::Ptr &dblock, const std::string &mesg)
    : Exception(mesg), dblock_(dblock) {}

DataBlockError::~DataBlockError() throw() {}

DataBlock::Ptr
DataBlockError::dblock() const {
    return dblock_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FunctionError
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FunctionError::FunctionError(const Function::Ptr &function, const std::string &mesg)
    : Exception(mesg), function_(function) {}

FunctionError::~FunctionError() throw() {}

Function::Ptr
FunctionError::function() const {
    return function_;
}

} // namespace
} // namespace
} // namespace

#endif
