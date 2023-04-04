#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmConstantExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Sawyer::Container::BitVector const&
SgAsmConstantExpression::get_bitVector() const {
    return p_bitVector;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
Sawyer::Container::BitVector&
SgAsmConstantExpression::get_bitVector() {
    return p_bitVector;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmConstantExpression::set_bitVector(Sawyer::Container::BitVector const& x) {
    this->p_bitVector = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmConstantExpression::~SgAsmConstantExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmConstantExpression::SgAsmConstantExpression() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmConstantExpression::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
