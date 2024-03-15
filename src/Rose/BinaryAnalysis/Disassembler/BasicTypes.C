#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>

#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch64.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/Jvm.h>
#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/Disassembler/Mips.h>
#include <Rose/BinaryAnalysis/Disassembler/Null.h>
#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>
#include <Rose/Diagnostics.h>

#include <Sawyer/Synchronization.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

Sawyer::Message::Facility mlog;

void initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Disassembler");
        mlog.comment("decoding machine language instructions");
    }
}

const std::string&
name(const Base::Ptr &disassembler) {
    return disassembler->name();
}

} // namespace
} // namespace
} // namespace
#endif
