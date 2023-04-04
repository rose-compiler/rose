#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmMemoryReferenceExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmMemoryReferenceExpression::get_address() const {
    return p_address;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMemoryReferenceExpression::set_address(SgAsmExpression* const& x) {
    this->p_address = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmMemoryReferenceExpression::get_segment() const {
    return p_segment;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMemoryReferenceExpression::set_segment(SgAsmExpression* const& x) {
    this->p_segment = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmMemoryReferenceExpression::~SgAsmMemoryReferenceExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmMemoryReferenceExpression::SgAsmMemoryReferenceExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_address(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_segment(nullptr) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmMemoryReferenceExpression
//    property=segment          class=SgAsmMemoryReferenceExpression
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmMemoryReferenceExpression::SgAsmMemoryReferenceExpression(SgAsmExpression* const& address,
                                                               SgAsmExpression* const& segment)
    : p_address(address)
    , p_segment(segment) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMemoryReferenceExpression::initializeProperties() {
    p_address = nullptr;
    p_segment = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
