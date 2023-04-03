#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmIndirectRegisterExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::RegisterDescriptor const&
SgAsmIndirectRegisterExpression::get_stride() const {
    return p_stride;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmIndirectRegisterExpression::set_stride(Rose::BinaryAnalysis::RegisterDescriptor const& x) {
    this->p_stride = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::RegisterDescriptor const&
SgAsmIndirectRegisterExpression::get_offset() const {
    return p_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmIndirectRegisterExpression::set_offset(Rose::BinaryAnalysis::RegisterDescriptor const& x) {
    this->p_offset = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmIndirectRegisterExpression::get_index() const {
    return p_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmIndirectRegisterExpression::set_index(size_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmIndirectRegisterExpression::get_modulus() const {
    return p_modulus;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmIndirectRegisterExpression::set_modulus(size_t const& x) {
    this->p_modulus = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmIndirectRegisterExpression::~SgAsmIndirectRegisterExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmIndirectRegisterExpression::SgAsmIndirectRegisterExpression()
    : p_index(0)
    , p_modulus(0) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=descriptor       class=SgAsmRegisterReferenceExpression
//    property=stride           class=SgAsmIndirectRegisterExpression
//    property=offset           class=SgAsmIndirectRegisterExpression
//    property=index            class=SgAsmIndirectRegisterExpression
//    property=modulus          class=SgAsmIndirectRegisterExpression
#line 313 "src/Rosebud/RosettaGenerator.C"
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

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmIndirectRegisterExpression::initializeProperties() {
    p_index = 0;
    p_modulus = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
