//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmMemoryReferenceExpression                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmExpression* const&
SgAsmMemoryReferenceExpression::get_address() const {
    return p_address;
}

void
SgAsmMemoryReferenceExpression::set_address(SgAsmExpression* const& x) {
    this->p_address = x;
    set_isModified(true);
}

SgAsmExpression* const&
SgAsmMemoryReferenceExpression::get_segment() const {
    return p_segment;
}

void
SgAsmMemoryReferenceExpression::set_segment(SgAsmExpression* const& x) {
    this->p_segment = x;
    set_isModified(true);
}

SgAsmMemoryReferenceExpression::~SgAsmMemoryReferenceExpression() {
    destructorHelper();
}

SgAsmMemoryReferenceExpression::SgAsmMemoryReferenceExpression()
    : p_address(nullptr)
    , p_segment(nullptr) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmMemoryReferenceExpression
//    property=segment          class=SgAsmMemoryReferenceExpression
SgAsmMemoryReferenceExpression::SgAsmMemoryReferenceExpression(SgAsmExpression* const& address,
                                                               SgAsmExpression* const& segment)
    : p_address(address)
    , p_segment(segment) {}

void
SgAsmMemoryReferenceExpression::initializeProperties() {
    p_address = nullptr;
    p_segment = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
