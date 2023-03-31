//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmValueExpression                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmValueExpression* const&
SgAsmValueExpression::get_unfolded_expression_tree() const {
    return p_unfolded_expression_tree;
}

void
SgAsmValueExpression::set_unfolded_expression_tree(SgAsmValueExpression* const& x) {
    this->p_unfolded_expression_tree = x;
    set_isModified(true);
}

unsigned short const&
SgAsmValueExpression::get_bit_offset() const {
    return p_bit_offset;
}

void
SgAsmValueExpression::set_bit_offset(unsigned short const& x) {
    this->p_bit_offset = x;
    set_isModified(true);
}

unsigned short const&
SgAsmValueExpression::get_bit_size() const {
    return p_bit_size;
}

void
SgAsmValueExpression::set_bit_size(unsigned short const& x) {
    this->p_bit_size = x;
    set_isModified(true);
}

SgSymbol* const&
SgAsmValueExpression::get_symbol() const {
    return p_symbol;
}

void
SgAsmValueExpression::set_symbol(SgSymbol* const& x) {
    this->p_symbol = x;
    set_isModified(true);
}

SgAsmValueExpression::~SgAsmValueExpression() {
    destructorHelper();
}

SgAsmValueExpression::SgAsmValueExpression()
    : p_unfolded_expression_tree(nullptr)
    , p_bit_offset(0)
    , p_bit_size(0)
    , p_symbol(nullptr) {}

void
SgAsmValueExpression::initializeProperties() {
    p_unfolded_expression_tree = nullptr;
    p_bit_offset = 0;
    p_bit_size = 0;
    p_symbol = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
