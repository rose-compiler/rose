#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/SharedMemory.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/SValue.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemoryContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SharedMemoryContext::~SharedMemoryContext() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedMemory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SharedMemory::SharedMemory() {}

SharedMemory::~SharedMemory() {}

SharedMemory::Ptr
SharedMemory::instance() {
    return Ptr(new SharedMemory);
}

Sawyer::Optional<uint8_t>
SharedMemory::previousReadConcreteAtOffset(size_t offset) const {
    return offset < prevReadConcrete_.size() ? prevReadConcrete_[offset] : Sawyer::Nothing();
}

void
SharedMemory::previousReadConcreteAtOffset(size_t offset, const Sawyer::Optional<uint8_t> &byte) {
    if (offset >= prevReadConcrete_.size())
        prevReadConcrete_.resize(offset + 1);
    prevReadConcrete_[offset] = byte;
}

SymbolicExpr::Ptr
SharedMemory::previousReadSymbolicAtOffset(size_t offset) const {
    return offset < prevReadSymbolic_.size() ? prevReadSymbolic_[offset] : SymbolicExpr::Ptr();
}

void
SharedMemory::previousReadSymbolicAtOffset(size_t offset, const SymbolicExpr::Ptr &byte) {
    if (offset >= prevReadSymbolic_.size())
        prevReadSymbolic_.resize(offset + 1);
    prevReadSymbolic_[offset] = byte;
}

const SharedMemory::Callbacks&
SharedMemory::callbacks() const {
    return callbacks_;
}

SharedMemory::Callbacks&
SharedMemory::callbacks() {
    return callbacks_;
}

} // namespace
} // namespace
} // namespace

#endif
