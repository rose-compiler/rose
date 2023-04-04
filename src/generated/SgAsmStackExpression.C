#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStackExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmStackExpression::get_stack_position() const {
    return p_stack_position;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStackExpression::set_stack_position(int const& x) {
    this->p_stack_position = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmStackExpression::~SgAsmStackExpression() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmStackExpression::SgAsmStackExpression()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_stack_position(0) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=stack_position   class=SgAsmStackExpression
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmStackExpression::SgAsmStackExpression(int const& stack_position)
    : p_stack_position(stack_position) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStackExpression::initializeProperties() {
    p_stack_position = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
