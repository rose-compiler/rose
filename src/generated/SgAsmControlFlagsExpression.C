//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmControlFlagsExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

unsigned long const&
SgAsmControlFlagsExpression::get_bit_flags() const {
    return p_bit_flags;
}

void
SgAsmControlFlagsExpression::set_bit_flags(unsigned long const& x) {
    this->p_bit_flags = x;
    set_isModified(true);
}

SgAsmControlFlagsExpression::~SgAsmControlFlagsExpression() {
    destructorHelper();
}

SgAsmControlFlagsExpression::SgAsmControlFlagsExpression()
    : p_bit_flags(0) {}

void
SgAsmControlFlagsExpression::initializeProperties() {
    p_bit_flags = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
