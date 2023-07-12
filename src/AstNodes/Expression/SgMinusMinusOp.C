#include <sage3basic.h>

SgMinusMinusOp::SgMinusMinusOp(Sg_File_Info * fi, SgExpression * exp, Sgop_mode a_mode)
   : SgUnaryOp(fi, exp, NULL)
   {
     set_mode(a_mode);
     if (exp)
          exp->set_parent(this);
   }

SgMinusMinusOp::SgMinusMinusOp(SgExpression * exp, Sgop_mode a_mode)
   : SgUnaryOp(exp, NULL)
   {
     set_mode(a_mode);
     if (exp)
          exp->set_parent(this);
   }

void
SgMinusMinusOp::post_construction_initialization()
   {
     set_mode(SgUnaryOp::prefix);
   }
