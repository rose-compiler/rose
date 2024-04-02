//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmMemoryReferenceExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmMemoryReferenceExpression_IMPL
#include <sage3basic.h>

SgAsmExpression* const&
SgAsmMemoryReferenceExpression::get_address() const {
    return p_address;
}

void
SgAsmMemoryReferenceExpression::set_address(SgAsmExpression* const& x) {
    changeChildPointer(this->p_address, const_cast<SgAsmExpression*&>(x));
    set_isModified(true);
}

SgAsmExpression* const&
SgAsmMemoryReferenceExpression::get_segment() const {
    return p_segment;
}

void
SgAsmMemoryReferenceExpression::set_segment(SgAsmExpression* const& x) {
    changeChildPointer(this->p_segment, const_cast<SgAsmExpression*&>(x));
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
    : p_address(initParentPointer(address, this))
    , p_segment(initParentPointer(segment, this)) {}

void
SgAsmMemoryReferenceExpression::initializeProperties() {
    p_address = nullptr;
    p_segment = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
