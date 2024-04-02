//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmIndirectRegisterExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmIndirectRegisterExpression_IMPL
#include <sage3basic.h>

Rose::BinaryAnalysis::RegisterDescriptor const&
SgAsmIndirectRegisterExpression::get_stride() const {
    return p_stride;
}

void
SgAsmIndirectRegisterExpression::set_stride(Rose::BinaryAnalysis::RegisterDescriptor const& x) {
    this->p_stride = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::RegisterDescriptor const&
SgAsmIndirectRegisterExpression::get_offset() const {
    return p_offset;
}

void
SgAsmIndirectRegisterExpression::set_offset(Rose::BinaryAnalysis::RegisterDescriptor const& x) {
    this->p_offset = x;
    set_isModified(true);
}

size_t const&
SgAsmIndirectRegisterExpression::get_index() const {
    return p_index;
}

void
SgAsmIndirectRegisterExpression::set_index(size_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

size_t const&
SgAsmIndirectRegisterExpression::get_modulus() const {
    return p_modulus;
}

void
SgAsmIndirectRegisterExpression::set_modulus(size_t const& x) {
    this->p_modulus = x;
    set_isModified(true);
}

SgAsmIndirectRegisterExpression::~SgAsmIndirectRegisterExpression() {
    destructorHelper();
}

SgAsmIndirectRegisterExpression::SgAsmIndirectRegisterExpression()
    : p_index(0)
    , p_modulus(0) {}

// The association between constructor arguments and their classes:
//    property=descriptor       class=SgAsmRegisterReferenceExpression
//    property=stride           class=SgAsmIndirectRegisterExpression
//    property=offset           class=SgAsmIndirectRegisterExpression
//    property=index            class=SgAsmIndirectRegisterExpression
//    property=modulus          class=SgAsmIndirectRegisterExpression
SgAsmIndirectRegisterExpression::SgAsmIndirectRegisterExpression(Rose::BinaryAnalysis::RegisterDescriptor const& descriptor,
                                                                 Rose::BinaryAnalysis::RegisterDescriptor const& stride,
                                                                 Rose::BinaryAnalysis::RegisterDescriptor const& offset,
                                                                 size_t const& index,
                                                                 size_t const& modulus)
    : SgAsmRegisterReferenceExpression(descriptor)
    , p_stride(stride)
    , p_offset(offset)
    , p_index(index)
    , p_modulus(modulus) {}

void
SgAsmIndirectRegisterExpression::initializeProperties() {
    p_index = 0;
    p_modulus = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
