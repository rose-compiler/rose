#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStaticData            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmStaticData::get_raw_bytes() const {
    return p_raw_bytes;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStaticData::set_raw_bytes(SgUnsignedCharList const& x) {
    this->p_raw_bytes = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmStaticData::~SgAsmStaticData() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmStaticData::SgAsmStaticData() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmStaticData::SgAsmStaticData(rose_addr_t const& address)
    : SgAsmStatement(address) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStaticData::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
