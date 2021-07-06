#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>

#include <Rose/BinaryAnalysis/Concolic/Architecture.h>
#include <Rose/BinaryAnalysis/Concolic/ExecutionEvent.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SyscallContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SyscallContext::~SyscallContext() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SystemCall
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SystemCall::SystemCall() {}

SystemCall::~SystemCall() {}

SystemCallPtr
SystemCall::instance() {
    return Ptr(new SystemCall);
}

const Sawyer::Optional<uint64_t>&
SystemCall::previousReturnConcrete() const {
    return prevReturnConcrete_;
}

void
SystemCall::previousReturnConcrete(uint64_t value) {
    prevReturnConcrete_ = value;
}

SymbolicExpr::Ptr
SystemCall::previousReturnSymbolic() const {
    return prevReturnSymbolic_;
}

void
SystemCall::previousReturnSymbolic(const SymbolicExpr::Ptr &value) {
    prevReturnSymbolic_ = value;
}

const SystemCall::Callbacks&
SystemCall::callbacks() const {
    return callbacks_;
}

SystemCall::Callbacks&
SystemCall::callbacks() {
    return callbacks_;
}

} // namespace
} // namespace
} // namespace

#endif
