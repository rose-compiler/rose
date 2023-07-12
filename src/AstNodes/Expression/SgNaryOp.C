#include <sage3basic.h>

SgExpressionPtrList&
SgNaryOp::get_operands()
   { return p_operands; }

const SgExpressionPtrList&
SgNaryOp::get_operands() const
   { return p_operands; }

VariantTList&
SgNaryOp::get_operators()
   { return p_operators; }

const VariantTList&
SgNaryOp::get_operators() const
   { return p_operators; }

void
SgNaryOp::append_operation(VariantT sg_operator, SgExpression* operand) {
    ROSE_ASSERT(operand != NULL);
    get_operands().push_back(operand);
    get_operators().push_back(sg_operator);
    operand->set_parent(this);
}

int
SgNaryOp::replace_expression(SgExpression *o, SgExpression *n) {
    SgExpressionPtrList::iterator i;
    for (i = get_operands().begin(); i != get_operands().end(); ++i) {
        if( (*i) == o) {
            *i = n;
            return 1;
        }
    }
    return 0;
}

int
SgNaryOp::length() const {
     return get_operands().size();
}

SgExpression*
SgNaryOp::get_next(int& i) const {
    return get_operands()[i++];
}

bool
SgNaryOp::empty() const {
    return get_operands().empty();
}
