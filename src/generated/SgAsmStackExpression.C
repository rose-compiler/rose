//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStackExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

int const&
SgAsmStackExpression::get_stack_position() const {
    return p_stack_position;
}

void
SgAsmStackExpression::set_stack_position(int const& x) {
    this->p_stack_position = x;
    set_isModified(true);
}

SgAsmStackExpression::~SgAsmStackExpression() {
    destructorHelper();
}

SgAsmStackExpression::SgAsmStackExpression()
    : p_stack_position(0) {}

// The association between constructor arguments and their classes:
//    property=stack_position   class=SgAsmStackExpression
SgAsmStackExpression::SgAsmStackExpression(int const& stack_position)
    : p_stack_position(stack_position) {}

void
SgAsmStackExpression::initializeProperties() {
    p_stack_position = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
