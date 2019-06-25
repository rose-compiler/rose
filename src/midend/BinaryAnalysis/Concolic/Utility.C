#include <sage3basic.h>

#include <BinaryConcolic.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

Sawyer::Message::Facility mlog;

// class method
void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Concolic");
        mlog.comment("concolic testing");
    }
}

} // namespace
} // namespace
} // namespace
