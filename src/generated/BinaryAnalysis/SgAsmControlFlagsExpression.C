//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmControlFlagsExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmControlFlagsExpression_IMPL
#include <sage3basic.h>

unsigned long const&
SgAsmControlFlagsExpression::get_bitFlags() const {
    return p_bitFlags;
}

void
SgAsmControlFlagsExpression::set_bitFlags(unsigned long const& x) {
    this->p_bitFlags = x;
    set_isModified(true);
}

SgAsmControlFlagsExpression::~SgAsmControlFlagsExpression() {
    destructorHelper();
}

SgAsmControlFlagsExpression::SgAsmControlFlagsExpression()
    : p_bitFlags(0) {}

void
SgAsmControlFlagsExpression::initializeProperties() {
    p_bitFlags = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
