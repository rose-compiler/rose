#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Architecture/Cil.h>

#include <Rose/BinaryAnalysis/Disassembler/Cil.h>
#include <Rose/BinaryAnalysis/Unparser/Cil.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Cil::Cil()
    : Base("cil", 2, ByteOrder::ORDER_MSB) {}

Cil::~Cil() {}

Cil::Ptr
Cil::instance() {
    return Ptr(new Cil);
}

RegisterDictionary::Ptr
Cil::registerDictionary() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!registerDictionary_.isCached())
        registerDictionary_ = RegisterDictionary::instance(name());
    return registerDictionary_.get();
}

bool
Cil::matchesHeader(SgAsmGenericHeader *header) const {
    ASSERT_not_null(header);
    return header->get_sectionsByName("CLR Runtime Header").size() > 0;
}

Disassembler::Base::Ptr
Cil::newInstructionDecoder() const {
    return Disassembler::Cil::instance(shared_from_this());
}

Unparser::Base::Ptr
Cil::newUnparser() const {
    return Unparser::Cil::instance(shared_from_this());
}

} // namespace
} // namespace
} // namespace

#endif
