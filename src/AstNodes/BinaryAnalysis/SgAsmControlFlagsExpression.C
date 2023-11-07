#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

unsigned long
SgAsmControlFlagsExpression::get_bit_flags() const {
    return get_bitFlags();
}

void
SgAsmControlFlagsExpression::set_bit_flags(unsigned long x) {
    set_bitFlags(x);
}

#endif
