#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <InstructionSemantics2.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {


using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::InstructionSemantics2");
        mlog.comment("evaluating instructions based on their behaviors");
    }
}

} // namespace
} // namespace
} // namespace

#endif
