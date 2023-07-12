#include <sage3basic.h>

SgPlusPlusOp::SgPlusPlusOp(Sg_File_Info * fi, SgExpression * exp, Sgop_mode a_mode)
   : SgUnaryOp(fi, exp, NULL)
   {
     set_mode(a_mode);
     if (exp)
          exp->set_parent(this);
   }

SgPlusPlusOp::SgPlusPlusOp( SgExpression * exp, Sgop_mode a_mode)
   : SgUnaryOp(exp, NULL)
   {
     set_mode(a_mode);
     if (exp)
          exp->set_parent(this);
   }

void
SgPlusPlusOp::post_construction_initialization()
   {
     set_mode(SgUnaryOp::prefix);
   }
