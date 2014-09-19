#ifndef	OFP_EXPR_H
#define	OFP_EXPR_H

#include "traversal.hpp"
#include <sstream>

namespace OFP {

class IntrinsicOperator : public Node
{
 public:
    IntrinsicOperator()
      {
         pEquivOp = NULL;
         pOrOp = NULL;
         pAndOp = NULL;
         pNotOp = NULL;
         pRelOp = NULL;
         pConcatOp = NULL;
         pAddOp = NULL;
         pMultOp = NULL;
         pPowerOp = NULL;
      }
    ~IntrinsicOperator() {}

   enum OptionType
     {
        DEFAULT = 0,
        IntrinsicOperator_EO,
        IntrinsicOperator_OO,
        IntrinsicOperator_AO2,
        IntrinsicOperator_NO,
        IntrinsicOperator_RO,
        IntrinsicOperator_CO,
        IntrinsicOperator_AO1,
        IntrinsicOperator_MO,
        IntrinsicOperator_PO
     };

    IntrinsicOperator* newIntrinsicOperator()
      {
         IntrinsicOperator* node = new IntrinsicOperator();
         node->pEquivOp = pEquivOp;  pEquivOp = NULL;
         node->pOrOp = pOrOp;  pOrOp = NULL;
         node->pAndOp = pAndOp;  pAndOp = NULL;
         node->pNotOp = pNotOp;  pNotOp = NULL;
         node->pRelOp = pRelOp;  pRelOp = NULL;
         node->pConcatOp = pConcatOp;  pConcatOp = NULL;
         node->pAddOp = pAddOp;  pAddOp = NULL;
         node->pMultOp = pMultOp;  pMultOp = NULL;
         node->pPowerOp = pPowerOp;  pPowerOp = NULL;
         node->inheritPayload(this);
         return node;
      }

    EquivOp* getEquivOp() {return pEquivOp;}
    OrOp* getOrOp() {return pOrOp;}
    AndOp* getAndOp() {return pAndOp;}
    NotOp* getNotOp() {return pNotOp;}
    RelOp* getRelOp() {return pRelOp;}
    ConcatOp* getConcatOp() {return pConcatOp;}
    AddOp* getAddOp() {return pAddOp;}
    MultOp* getMultOp() {return pMultOp;}
    PowerOp* getPowerOp() {return pPowerOp;}

    void setEquivOp(EquivOp* equivop) {pEquivOp = equivop;}
    void setOrOp(OrOp* orop) {pOrOp = orop;}
    void setAndOp(AndOp* andop) {pAndOp = andop;}
    void setNotOp(NotOp* notop) {pNotOp = notop;}
    void setRelOp(RelOp* relop) {pRelOp = relop;}
    void setConcatOp(ConcatOp* concatop) {pConcatOp = concatop;}
    void setAddOp(AddOp* addop) {pAddOp = addop;}
    void setMultOp(MultOp* multop) {pMultOp = multop;}
    void setPowerOp(PowerOp* powerop) {pPowerOp = powerop;}

 private:
    EquivOp* pEquivOp;
    OrOp* pOrOp;
    AndOp* pAndOp;
    NotOp* pNotOp;
    RelOp* pRelOp;
    ConcatOp* pConcatOp;
    AddOp* pAddOp;
    MultOp* pMultOp;
    PowerOp* pPowerOp;
};

class Expr : public Node
{
 public:
    Expr()
      {
         pExpr1 = NULL;
         pExpr2 = NULL;
         pPrimary = NULL;
         pDefinedBinaryOp = NULL;
         pDefinedUnaryOp = NULL;
      }
   ~Expr();

   enum OptionType
     {
        DEFAULT = 0,
        Primary_ot,
        DefBinExpr_ot,
        NEQV_ot,
        EQV_ot,
        OR_ot,
        AND_ot,
        NOT_ot,
        GE_ot,
        GT_ot,
        LE_ot,
        LT_ot,
        NE_ot,
        EQ_ot,
        Concat_ot,
        Minus_ot,
        Plus_ot,
        UnaryMinus_ot,
        UnaryPlus_ot,
        Div_ot,
        Mult_ot,
        Power_ot,
        DefUnaryExpr_ot
     };

    Expr* newExpr()
      {
         Expr* node = new Expr();
         node->pExpr1 = pExpr1;  pExpr1 = NULL;
         node->pExpr2 = pExpr1;  pExpr2 = NULL;
         node->pPrimary = pPrimary;  pPrimary = NULL;
         node->pDefinedBinaryOp = pDefinedBinaryOp;  pDefinedBinaryOp = NULL;
         node->pDefinedUnaryOp = pDefinedUnaryOp;  pDefinedUnaryOp = NULL;
         node->inheritPayload(this);
         return node;
      }

    

    Expr* getExpr1() {return pExpr1;}
    Expr* getExpr2() {return pExpr2;}
    Primary* getPrimary() {return pPrimary;}
    DefinedBinaryOp* getDefinedBinaryOp() {return pDefinedBinaryOp;}
    DefinedUnaryOp* getDefinedUnaryOp() {return pDefinedUnaryOp;}

    void setExpr1(Expr* expr) {pExpr1 = expr;}
    void setExpr2(Expr* expr) {pExpr2 = expr;}
    void setPrimary(Primary* primary) {pPrimary = primary;}
    void setDefinedBinaryOp(DefinedBinaryOp* definedbinaryop) {pDefinedBinaryOp = definedbinaryop;}
    void setDefinedUnaryOp(DefinedUnaryOp* definedunaryop) {pDefinedUnaryOp = definedunaryop;}

 private:
    Expr* pExpr1;
    Expr* pExpr2;
    Primary* pPrimary;
    DefinedBinaryOp* pDefinedBinaryOp;
    DefinedUnaryOp* pDefinedUnaryOp;
};

} // namespace OFP

#endif
