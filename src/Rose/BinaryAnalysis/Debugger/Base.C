#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Debugger/Base.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Debugger/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>

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
    FilterAction operator()(Address) {
        return FilterAction();
    }
};

Base::Base() {}

Base::~Base() {}

Address
Base::executionAddress(ThreadId tid) {
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    const RegisterDescriptor REG_PC = disassembler()->architecture()->registerDictionary()->instructionPointerRegister();
    return readRegister(tid, REG_PC).toInteger();
}

void
Base::executionAddress(ThreadId tid, Address va) {
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    const RegisterDescriptor REG_PC = disassembler()->architecture()->registerDictionary()->instructionPointerRegister();
    writeRegister(tid, REG_PC, va);
}

Disassembler::Base::Ptr
Base::disassembler() {
    ASSERT_not_null(disassembler_);
    return disassembler_;
}

RegisterDictionary::Ptr
Base::registerDictionary() {
    ASSERT_not_null(disassembler());
    return disassembler()->architecture()->registerDictionary();
}

std::string
Base::registerName(RegisterDescriptor desc) {
    const std::string s = registerDictionary()->lookup(desc);
    return s.empty() ? desc.toString() : s;
}

Sawyer::Container::Trace<Address>
Base::trace() {
    DefaultTraceFilter filter;
    return trace(ThreadId::unspecified(), filter);
}

std::string
Base::readCString(Address va, size_t maxBytes) {
    std::string retval;
    while (maxBytes > 0) {
        uint8_t buf[32];
        size_t nRead = readMemory(va, std::min(maxBytes, sizeof buf), buf);
        if (0 == nRead)
            break;
        for (size_t i = 0; i < nRead; ++i) {
            if (0 == buf[i]) {
                return retval;                          // NUL terminated
            } else {
                retval += (char)buf[i];
            }
        }
        maxBytes -= nRead;
        va += nRead;
    }
    return retval;                                      // buffer overflow
}


} // namespace
} // namespace
} // namespace

#endif
