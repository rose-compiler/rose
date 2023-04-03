#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmValueExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmValueExpression* const&
SgAsmValueExpression::get_unfolded_expression_tree() const {
    return p_unfolded_expression_tree;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmValueExpression::set_unfolded_expression_tree(SgAsmValueExpression* const& x) {
    this->p_unfolded_expression_tree = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned short const&
SgAsmValueExpression::get_bit_offset() const {
    return p_bit_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmValueExpression::set_bit_offset(unsigned short const& x) {
    this->p_bit_offset = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned short const&
SgAsmValueExpression::get_bit_size() const {
    return p_bit_size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmValueExpression::set_bit_size(unsigned short const& x) {
    this->p_bit_size = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgSymbol* const&
SgAsmValueExpression::get_symbol() const {
    return p_symbol;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmValueExpression::set_symbol(SgSymbol* const& x) {
    this->p_symbol = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmValueExpression::~SgAsmValueExpression() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmValueExpression::SgAsmValueExpression()
    : p_unfolded_expression_tree(nullptr)
    , p_bit_offset(0)
    , p_bit_size(0)
    , p_symbol(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmValueExpression::initializeProperties() {
    p_unfolded_expression_tree = nullptr;
    p_bit_offset = 0;
    p_bit_size = 0;
    p_symbol = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
