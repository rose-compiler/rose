#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmControlFlagsExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned long const&
SgAsmControlFlagsExpression::get_bit_flags() const {
    return p_bit_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmControlFlagsExpression::set_bit_flags(unsigned long const& x) {
    this->p_bit_flags = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmControlFlagsExpression::~SgAsmControlFlagsExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmControlFlagsExpression::SgAsmControlFlagsExpression()
    : p_bit_flags(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmControlFlagsExpression::initializeProperties() {
    p_bit_flags = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
