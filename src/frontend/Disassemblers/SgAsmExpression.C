#include "sage3basic.h"

/** Width of expression in bits, according to its type. */
size_t
SgAsmExpression::get_nBits() const {
    SgAsmType *type = get_type();
    ASSERT_not_null2(type, "expression has no type");
    return type->get_nBits();
}
