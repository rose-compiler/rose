#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "OFPExpr.hpp"
#include "ASTBuilder.hpp"
#include <assert.h>
#include <string>
#include <iostream>
#include <sstream>

extern OFP::ASTBuilder * ast;

OFP::Expr::~Expr()
{
   if (pExpr1)            delete pExpr1;
   if (pExpr2)            delete pExpr2;
   if (pPrimary)          delete pPrimary;
   if (pDefinedBinaryOp)  delete pDefinedBinaryOp;
   if (pDefinedUnaryOp)   delete pDefinedUnaryOp;
}

//========================================================================================
// R309 intrinsic-operator
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_IntrinsicOperator(ATerm term, OFP::IntrinsicOperator* IntrinsicOperator)
{
#ifdef DEBUG_PRINT
   printf("IntrinsicOperator: %s\n", ATwriteToString(term));
#endif

#ifdef NOT_YET
 OFP::EquivOp EquivOp;
 if (ATmatch(term, "IntrinsicOperator_EO(<term>)", &EquivOp.term)) {

      if (ofp_traverse_EquivOp(EquivOp.term, &EquivOp)) {
         // MATCHED EquivOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_EO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::OrOp OrOp;
 if (ATmatch(term, "IntrinsicOperator_OO(<term>)", &OrOp.term)) {

      if (ofp_traverse_OrOp(OrOp.term, &OrOp)) {
         // MATCHED OrOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_OO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::AndOp AndOp;
 if (ATmatch(term, "IntrinsicOperator_AO2(<term>)", &AndOp.term)) {

      if (ofp_traverse_AndOp(AndOp.term, &AndOp)) {
         // MATCHED AndOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_AO2

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::NotOp NotOp;
 if (ATmatch(term, "IntrinsicOperator_NO(<term>)", &NotOp.term)) {

      if (ofp_traverse_NotOp(NotOp.term, &NotOp)) {
         // MATCHED NotOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_NO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::RelOp RelOp;
 if (ATmatch(term, "IntrinsicOperator_RO(<term>)", &RelOp.term)) {

      if (ofp_traverse_RelOp(RelOp.term, &RelOp)) {
         // MATCHED RelOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_RO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::ConcatOp ConcatOp;
 if (ATmatch(term, "IntrinsicOperator_CO(<term>)", &ConcatOp.term)) {

      if (ofp_traverse_ConcatOp(ConcatOp.term, &ConcatOp)) {
         // MATCHED ConcatOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_CO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::AddOp AddOp;
 if (ATmatch(term, "IntrinsicOperator_AO1(<term>)", &AddOp.term)) {

      if (ofp_traverse_AddOp(AddOp.term, &AddOp)) {
         // MATCHED AddOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_AO1

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::MultOp MultOp;
 if (ATmatch(term, "IntrinsicOperator_MO(<term>)", &MultOp.term)) {

      if (ofp_traverse_MultOp(MultOp.term, &MultOp)) {
         // MATCHED MultOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_MO

   return ATtrue;
 }
#endif

#ifdef NOT_YET
 OFP::PowerOp PowerOp;
 if (ATmatch(term, "IntrinsicOperator_PO(<term>)", &PowerOp.term)) {

      if (ofp_traverse_PowerOp(PowerOp.term, &PowerOp)) {
         // MATCHED PowerOp
      } else return ATfalse;

   // MATCHED IntrinsicOperator_PO

   return ATtrue;
 }
#endif

 return ATfalse;
}

//========================================================================================
// R722 expr
//----------------------------------------------------------------------------------------
ATbool ofp_traverse_Expr(ATerm term, OFP::Expr* Expr)
{
#ifdef DEBUG_PRINT
   printf("Expr(W): %s\n", ATwriteToString(term));
#endif

 OFP::Expr Expr1, Expr2;
 OFP::DefinedBinaryOp DefinedBinaryOp;
 if (ATmatch(term, "DefBinExpr(<term>,<term>,<term>)", &Expr1.term, &DefinedBinaryOp.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_DefinedBinaryOp(DefinedBinaryOp.term, &DefinedBinaryOp)) {
         // MATCHED DefinedBinaryOp
         Expr->setDefinedBinaryOp(DefinedBinaryOp.newDefinedBinaryOp());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED DefBinExpr
   Expr->setOptionType(OFP::Expr::DefBinExpr);

   //TODO-CER-2014.4.15 - implement defined binary operators
   //   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_DEFINED_BINARY, "");
   assert(0);

   return ATtrue;
 }

 if (ATmatch(term, "NotEqvExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED NotEqvExpr
   Expr->setOptionType(OFP::Expr::NotEqvExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_NEQV, ".NEQV.");

   return ATtrue;
 }

 if (ATmatch(term, "EqvExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED EqvExpr
   Expr->setOptionType(OFP::Expr::EqvExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_EQV, ".EQV.");

   return ATtrue;
 }

 if (ATmatch(term, "OrExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED OrExpr
   Expr->setOptionType(OFP::Expr::OrExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_OR, ".OR.");

   return ATtrue;
 }

 if (ATmatch(term, "AndExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED AndExpr
   Expr->setOptionType(OFP::Expr::AndExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_AND, ".AND.");

   return ATtrue;
 }

 if (ATmatch(term, "NotExpr(<term>)", &Expr1.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

   // MATCHED NotExpr
   Expr->setOptionType(OFP::Expr::NotExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_NOT, ".NOT.");

   return ATtrue;
 }

 if (ATmatch(term, "GE_Expr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED GE_Expr
   Expr->setOptionType(OFP::Expr::GE_Expr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_GE, ">=");

   return ATtrue;
 }

 if (ATmatch(term, "GT_Expr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED GT_Expr
   Expr->setOptionType(OFP::Expr::GT_Expr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_GT, ">");

   return ATtrue;
 }

 if (ATmatch(term, "LE_Expr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED LE_Expr
   Expr->setOptionType(OFP::Expr::LE_Expr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_LE, "<=");

   return ATtrue;
 }

 if (ATmatch(term, "LT_Expr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED LT_Expr
   Expr->setOptionType(OFP::Expr::LT_Expr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_LT, "<");

   return ATtrue;
 }

 if (ATmatch(term, "NE_Expr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED NE_Expr
   Expr->setOptionType(OFP::Expr::NE_Expr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_NE, "/=");

   return ATtrue;
 }

 if (ATmatch(term, "EQ_Expr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED EQ_Expr
   Expr->setOptionType(OFP::Expr::EQ_Expr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_EQ, "==");

   return ATtrue;
 }

 if (ATmatch(term, "ConcatExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED ConcatExpr
   Expr->setOptionType(OFP::Expr::ConcatExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_CONCAT, "//");

   return ATtrue;
 }

 if (ATmatch(term, "MinusExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED MinusExpr
   Expr->setOptionType(OFP::Expr::MinusExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_MINUS, "-");

   return ATtrue;
 }

 if (ATmatch(term, "PlusExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED PlusExpr
   Expr->setOptionType(OFP::Expr::PlusExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_PLUS, "+");

   return ATtrue;
 }

 if (ATmatch(term, "UnaryMinusExpr(<term>)", &Expr1.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

   // MATCHED UnaryMinusExpr
   Expr->setOptionType(OFP::Expr::UnaryMinusExpr);

   //TODO-CER-2014.4.15 - implement unary operators
   //   ast->build_UnaryOp(Expr, SgToken::FORTRAN_INTRINSIC_UNARY_MINUS, "-");
   assert(0);

   return ATtrue;
 }

 if (ATmatch(term, "UnaryPlusExpr(<term>)", &Expr1.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

   // MATCHED UnaryPlusExpr
   Expr->setOptionType(OFP::Expr::UnaryPlusExpr);

   //TODO-CER-2014.4.15 - implement unary operators
   //   ast->build_UnaryOp(Expr, SgToken::FORTRAN_INTRINSIC_UNARY_PLUS, "+");
   assert(0);

   return ATtrue;
 }

 if (ATmatch(term, "DivExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED DivExpr
   Expr->setOptionType(OFP::Expr::DivExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_DIVIDE, "/");

   return ATtrue;
 }

 if (ATmatch(term, "MultExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED MultExpr
   Expr->setOptionType(OFP::Expr::MultExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_TIMES, "*");

   return ATtrue;
 }

 if (ATmatch(term, "PowerExpr(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED PowerExpr
   Expr->setOptionType(OFP::Expr::PowerExpr);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_POWER, "**");

   return ATtrue;
 }

 OFP::DefinedUnaryOp DefinedUnaryOp;
 if (ATmatch(term, "DefUnaryExpr(<term>,<term>)", &DefinedUnaryOp.term, &Expr1.term)) {

      if (ofp_traverse_DefinedUnaryOp(DefinedUnaryOp.term, &DefinedUnaryOp)) {
         // MATCHED DefinedUnaryOp
         Expr->setDefinedUnaryOp(DefinedUnaryOp.newDefinedUnaryOp());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

   // MATCHED DefUnaryExpr
   Expr->setOptionType(OFP::Expr::DefUnaryExpr);

   //TODO-CER-2014.4.15 - implement defined unary operators
   //   ast->build_UnaryOp(Expr, SgToken::FORTRAN_INTRINSIC_DEFINED_UNARY, "");
   assert(0);

   return ATtrue;
 }

 OFP::Primary Primary;
 if (ATmatch(term, "Expr_P(<term>)", &Primary.term)) {

      if (ofp_traverse_Primary(Primary.term, &Primary)) {
         // MATCHED Primary
         Expr->setPrimary(Primary.newPrimary());
         Expr->inheritPayload(Expr->getPrimary());
      } else return ATfalse;

   // MATCHED Expr_P
   Expr->setOptionType(OFP::Expr::Expr_P);

   return ATtrue;
 }

 return ATfalse;
}
