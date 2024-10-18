#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmValueExpression.h>

SgAsmValueExpression*
SgAsmValueExpression::get_unfolded_expression_tree() const {
    return get_unfoldedExpression();
}

void
SgAsmValueExpression::set_unfolded_expression_tree(SgAsmValueExpression *x) {
    set_unfoldedExpression(x);
}

unsigned short
SgAsmValueExpression::get_bit_offset() const {
    return get_bitOffset();
}

void
SgAsmValueExpression::set_bit_offset(unsigned short x) {
    set_bitOffset(x);
}

unsigned short
SgAsmValueExpression::get_bit_size() const {
    return get_bitSize();
}

void
SgAsmValueExpression::set_bit_size(unsigned short x) {
    set_bitSize(x);
}

#endif
