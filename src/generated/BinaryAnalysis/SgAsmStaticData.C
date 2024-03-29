//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStaticData            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgUnsignedCharList const&
SgAsmStaticData::get_raw_bytes() const {
    return p_raw_bytes;
}

void
SgAsmStaticData::set_raw_bytes(SgUnsignedCharList const& x) {
    this->p_raw_bytes = x;
    set_isModified(true);
}

SgAsmStaticData::~SgAsmStaticData() {
    destructorHelper();
}

SgAsmStaticData::SgAsmStaticData() {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
SgAsmStaticData::SgAsmStaticData(rose_addr_t const& address)
    : SgAsmStatement(address) {}

void
SgAsmStaticData::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
