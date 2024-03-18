#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>

#include <Rose/Diagnostics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {


using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::InstructionSemantics");
        mlog.comment("evaluating instructions based on their behaviors");
    }
}

} // namespace
} // namespace
} // namespace

#endif
