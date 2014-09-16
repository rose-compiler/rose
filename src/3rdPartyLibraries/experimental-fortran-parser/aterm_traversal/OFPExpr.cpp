#define COMPILED_WITH_ROSE 1

#if COMPILED_WITH_ROSE
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"
#endif

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

   OFP::Primary Primary;
   if (ofp_traverse_Primary(term, &Primary)) {
      // MATCHED Primary
      Expr->setOptionType(OFP::Expr::Primary_ot);
      Expr->setPrimary(Primary.newPrimary());
      Expr->inheritPayload(Expr->getPrimary());
      return ATtrue;
   }

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
   Expr->setOptionType(OFP::Expr::DefBinExpr_ot);

   //TODO-CER-2014.4.15 - implement defined binary operators
   //   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_DEFINED_BINARY, "");
   assert(0);

   return ATtrue;
 }

 if (ATmatch(term, "NEQV(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED NEQV
   Expr->setOptionType(OFP::Expr::NEQV_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_NEQV, ".NEQV.");

   return ATtrue;
 }

 if (ATmatch(term, "EQV(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED EQV
   Expr->setOptionType(OFP::Expr::EQV_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_EQV, ".EQV.");

   return ATtrue;
 }

 if (ATmatch(term, "OR(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED OR
   Expr->setOptionType(OFP::Expr::OR_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_OR, ".OR.");

   return ATtrue;
 }

 if (ATmatch(term, "AND(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED AND
   Expr->setOptionType(OFP::Expr::AND_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_AND, ".AND.");

   return ATtrue;
 }

 if (ATmatch(term, "NOT(<term>)", &Expr1.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

   // MATCHED NOT
   Expr->setOptionType(OFP::Expr::NOT_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_NOT, ".NOT.");

   return ATtrue;
 }

 if (ATmatch(term, "GE(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED GE
   Expr->setOptionType(OFP::Expr::GE_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_GE, ">=");

   return ATtrue;
 }

 if (ATmatch(term, "GT(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED GT
   Expr->setOptionType(OFP::Expr::GT_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_GT, ">");

   return ATtrue;
 }

 if (ATmatch(term, "LE(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED LE
   Expr->setOptionType(OFP::Expr::LE_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_LE, "<=");

   return ATtrue;
 }

 if (ATmatch(term, "LT(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED LT
   Expr->setOptionType(OFP::Expr::LT_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_LT, "<");

   return ATtrue;
 }

 if (ATmatch(term, "NE(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED NE
   Expr->setOptionType(OFP::Expr::NE_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_NE, "/=");

   return ATtrue;
 }

 if (ATmatch(term, "EQ(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED EQ
   Expr->setOptionType(OFP::Expr::EQ_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_EQ, "==");

   return ATtrue;
 }

 if (ATmatch(term, "Concat(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED Concat
   Expr->setOptionType(OFP::Expr::Concat_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_CONCAT, "//");

   return ATtrue;
 }

 if (ATmatch(term, "Minus(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED Minus
   Expr->setOptionType(OFP::Expr::Minus_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_MINUS, "-");

   return ATtrue;
 }

 if (ATmatch(term, "Plus(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED Plus
   Expr->setOptionType(OFP::Expr::Plus_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_PLUS, "+");

   return ATtrue;
 }

 if (ATmatch(term, "UnaryMinus(<term>)", &Expr1.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

   // MATCHED UnaryMinus
   Expr->setOptionType(OFP::Expr::UnaryMinus_ot);

   //TODO-CER-2014.4.15 - implement unary operators
   //   ast->build_UnaryOp(Expr, SgToken::FORTRAN_INTRINSIC_UNARY_MINUS, "-");
   assert(0);

   return ATtrue;
 }

 if (ATmatch(term, "UnaryPlus(<term>)", &Expr1.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

   // MATCHED UnaryPlus
   Expr->setOptionType(OFP::Expr::UnaryPlus_ot);

   //TODO-CER-2014.4.15 - implement unary operators
   //   ast->build_UnaryOp(Expr, SgToken::FORTRAN_INTRINSIC_UNARY_PLUS, "+");
   assert(0);

   return ATtrue;
 }

 if (ATmatch(term, "Div(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED Div
   Expr->setOptionType(OFP::Expr::Div_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_DIVIDE, "/");

   return ATtrue;
 }

 if (ATmatch(term, "Mult(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED Mult
   Expr->setOptionType(OFP::Expr::Mult_ot);

   ast->build_BinaryOp(Expr, SgToken::FORTRAN_INTRINSIC_TIMES, "*");

   return ATtrue;
 }

 if (ATmatch(term, "Power(<term>,<term>)", &Expr1.term, &Expr2.term)) {

      if (ofp_traverse_Expr(Expr1.term, &Expr1)) {
         // MATCHED Expr
         Expr->setExpr1(Expr1.newExpr());
      } else return ATfalse;

      if (ofp_traverse_Expr(Expr2.term, &Expr2)) {
         // MATCHED Expr
         Expr->setExpr2(Expr2.newExpr());
      } else return ATfalse;

   // MATCHED Power
   Expr->setOptionType(OFP::Expr::Power_ot);

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
   Expr->setOptionType(OFP::Expr::DefUnaryExpr_ot);

   //TODO-CER-2014.4.15 - implement defined unary operators
   //   ast->build_UnaryOp(Expr, SgToken::FORTRAN_INTRINSIC_DEFINED_UNARY, "");
   assert(0);

   return ATtrue;
 }

 return ATfalse;
}
