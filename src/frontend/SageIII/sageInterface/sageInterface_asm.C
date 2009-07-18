#include "rose.h"
#include <stdint.h>
#include <iostream>

uint64_t
SageInterface::getAsmConstant(SgAsmValueExpression *expr)
{
    switch (expr->variantT()) {
        case V_SgAsmByteValueExpression:
            return isSgAsmByteValueExpression(expr)->get_value();
        case V_SgAsmWordValueExpression:
            return isSgAsmWordValueExpression(expr)->get_value();
        case V_SgAsmDoubleWordValueExpression:
            return isSgAsmDoubleWordValueExpression(expr)->get_value();
        case V_SgAsmQuadWordValueExpression:
            return isSgAsmQuadWordValueExpression(expr)->get_value();
        default: 
            std::cerr << "Bad variant " << expr->class_name() << " in " <<__func__ << std::endl;
            abort();
    }
}

int64_t
SageInterface::getAsmSignedConstant(SgAsmValueExpression *expr) 
{
    switch (expr->variantT()) {
        case V_SgAsmByteValueExpression:
            return IntegerOps::signExtend<8, 64>((uint64_t)isSgAsmByteValueExpression(expr)->get_value());
        case V_SgAsmWordValueExpression:
            return IntegerOps::signExtend<16, 64>((uint64_t)isSgAsmWordValueExpression(expr)->get_value());
        case V_SgAsmDoubleWordValueExpression:
            return IntegerOps::signExtend<32, 64>((uint64_t)isSgAsmDoubleWordValueExpression(expr)->get_value());
        case V_SgAsmQuadWordValueExpression:
            return (int64_t)(isSgAsmQuadWordValueExpression(expr)->get_value());
        default:
            std::cerr <<"Bad variant " <<expr->class_name() <<" in " <<__func__ <<std::endl;
            abort();
    }
}

