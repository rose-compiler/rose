#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmMemoryReferenceExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmMemoryReferenceExpression::get_address() const {
    return p_address;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMemoryReferenceExpression::set_address(SgAsmExpression* const& x) {
    this->p_address = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExpression* const&
SgAsmMemoryReferenceExpression::get_segment() const {
    return p_segment;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMemoryReferenceExpression::set_segment(SgAsmExpression* const& x) {
    this->p_segment = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmMemoryReferenceExpression::~SgAsmMemoryReferenceExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmMemoryReferenceExpression::SgAsmMemoryReferenceExpression()
    : p_address(nullptr)
    , p_segment(nullptr) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmMemoryReferenceExpression
//    property=segment          class=SgAsmMemoryReferenceExpression
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmMemoryReferenceExpression::SgAsmMemoryReferenceExpression(SgAsmExpression* const& address,
                                                               SgAsmExpression* const& segment)
    : p_address(address)
    , p_segment(segment) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMemoryReferenceExpression::initializeProperties() {
    p_address = nullptr;
    p_segment = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
