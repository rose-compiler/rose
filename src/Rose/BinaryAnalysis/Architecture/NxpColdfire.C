#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/NxpColdfire.h>

#include <Rose/BinaryAnalysis/Disassembler/M68k.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

NxpColdfire::NxpColdfire()
    : Base("nxp-coldfire", 4, ByteOrder::ORDER_MSB) {}

NxpColdfire::~NxpColdfire() {}

NxpColdfire::Ptr
NxpColdfire::instance() {
    return Ptr(new NxpColdfire);
}

RegisterDictionary::Ptr
NxpColdfire::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instanceColdfireEmac();
    return registerDictionary_.get();
}

bool
NxpColdfire::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    return (isa & SgAsmExecutableFileFormat::ISA_FAMILY_MASK) == SgAsmExecutableFileFormat::ISA_M68K_Family;
}

Disassembler::Base::Ptr
NxpColdfire::newInstructionDecoder() const {
    return Disassembler::M68k::instance(shared_from_this(), m68k_freescale_cpu32);
}

} // namespace
} // namespace
} // namespace

#endif
