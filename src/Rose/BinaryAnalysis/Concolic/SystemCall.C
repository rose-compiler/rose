#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Concolic/SystemCall.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

SystemCall::SystemCall() {}

SystemCall::~SystemCall() {}

SystemCallPtr
SystemCall::instance() {
    return Ptr(new SystemCall);
}

bool
SystemCall::exitsProcess() const {
    return exitsProcess_;
}

void
SystemCall::exitsProcess(bool b) {
    exitsProcess_ = b;
}

bool
SystemCall::isConstantReturn() const {
    return constantReturn_.enabled;
}

void
SystemCall::isConstantReturn(bool b) {
    constantReturn_.enabled = b;
    if (!b) {
        constantReturn_.concrete = Sawyer::Nothing();
        constantReturn_.symbolic = SymbolicExpr::Ptr();
    }
}

const Sawyer::Optional<uint64_t>&
SystemCall::constantReturnConcrete() const {
    return constantReturn_.concrete;
}

void
SystemCall::constantReturnConcrete(uint64_t value) {
    constantReturn_.enabled = true;
    constantReturn_.concrete = value;
}

SymbolicExpr::Ptr
SystemCall::constantReturnSymbolic() const {
    return constantReturn_.symbolic;
}

void
SystemCall::constantReturnSymbolic(const SymbolicExpr::Ptr &value) {
    ASSERT_not_null(value);
    constantReturn_.enabled = true;
    constantReturn_.symbolic = value;
}

} // namespace
} // namespace
} // namespace

#endif
