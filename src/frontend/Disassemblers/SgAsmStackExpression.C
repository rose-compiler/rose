#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

int
SgAsmStackExpression::get_stack_position() const {
    return get_stackPosition();
}

void
SgAsmStackExpression::set_stack_position(int x) {
    set_stackPosition(x);
}

#endif
