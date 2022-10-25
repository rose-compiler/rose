#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Debugger/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Debugger");
        mlog.comment("debugging other processes");
    }
}

struct DefaultTraceFilter {
    FilterAction operator()(rose_addr_t) {
        return FilterAction();
    }
};

Base::Base() {}

Base::~Base() {}

Sawyer::Container::Trace<rose_addr_t>
Base::trace() {
    DefaultTraceFilter filter;
    return trace(ThreadId::unspecified(), filter);
}

} // namespace
} // namespace
} // namespace

#endif
