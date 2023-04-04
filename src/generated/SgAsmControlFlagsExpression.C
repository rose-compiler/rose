#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmControlFlagsExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmControlFlagsExpression::get_bit_flags() const {
    return p_bit_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmControlFlagsExpression::set_bit_flags(unsigned long const& x) {
    this->p_bit_flags = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmControlFlagsExpression::~SgAsmControlFlagsExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmControlFlagsExpression::SgAsmControlFlagsExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_bit_flags(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmControlFlagsExpression::initializeProperties() {
    p_bit_flags = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
