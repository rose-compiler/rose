//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStaticData            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmStaticData_IMPL
#include <SgAsmStaticData.h>

SgUnsignedCharList const&
SgAsmStaticData::get_rawBytes() const {
    return p_rawBytes;
}

void
SgAsmStaticData::set_rawBytes(SgUnsignedCharList const& x) {
    this->p_rawBytes = x;
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
