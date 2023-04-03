#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStackExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmStackExpression::get_stack_position() const {
    return p_stack_position;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStackExpression::set_stack_position(int const& x) {
    this->p_stack_position = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmStackExpression::~SgAsmStackExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmStackExpression::SgAsmStackExpression()
    : p_stack_position(0) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=stack_position   class=SgAsmStackExpression
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmStackExpression::SgAsmStackExpression(int const& stack_position)
    : p_stack_position(stack_position) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStackExpression::initializeProperties() {
    p_stack_position = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
