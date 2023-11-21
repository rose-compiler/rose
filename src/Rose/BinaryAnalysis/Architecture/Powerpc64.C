#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Powerpc64.h>

#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Powerpc64::Powerpc64(ByteOrder::Endianness byteOrder)
    : Base(ByteOrder::ORDER_MSB == byteOrder ? "ppc64-be" : (ByteOrder::ORDER_LSB == byteOrder ? "ppc64-el" : "ppc64"),
           8, byteOrder) {}

Powerpc64::~Powerpc64() {}

Powerpc64::Ptr
Powerpc64::instance(ByteOrder::Endianness byteOrder) {
    return Ptr(new Powerpc64(byteOrder));
}

RegisterDictionary::Ptr
Powerpc64::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instancePowerpc64();
    return registerDictionary_.get();
}

bool
Powerpc64::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    const SgAsmExecutableFileFormat::InsSetArchitecture isa = header->get_isa();
    SgAsmGenericFormat *fmt = header->get_executableFormat();
    ASSERT_not_null(fmt);
    return SgAsmExecutableFileFormat::ISA_PowerPC_64bit == isa && fmt->get_sex() == byteOrder();
}

Disassembler::Base::Ptr
Powerpc64::newInstructionDecoder() const {
    return Disassembler::Powerpc::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
