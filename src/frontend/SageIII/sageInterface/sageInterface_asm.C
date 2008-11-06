#include "rose.h"
#include <stdint.h>
#include <iostream>

using namespace std;

uint64_t SageInterface::getAsmConstant(SgAsmValueExpression* expr) {
  switch (expr->variantT()) {
    case V_SgAsmByteValueExpression: return isSgAsmByteValueExpression(expr)->get_value();
    case V_SgAsmWordValueExpression: return isSgAsmWordValueExpression(expr)->get_value();
    case V_SgAsmDoubleWordValueExpression: return isSgAsmDoubleWordValueExpression(expr)->get_value();
    case V_SgAsmQuadWordValueExpression: return isSgAsmQuadWordValueExpression(expr)->get_value();
    default: {cerr << "Bad variant " << expr->class_name() << " in getAsmConstant" << endl; abort();}
  }
}

