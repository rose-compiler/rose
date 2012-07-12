// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GDotBackEnd.h,v 1.2 2007-03-08 15:36:49 markus Exp $

#ifndef GDOTBACKEND_H
#define GDOTBACKEND_H

#include "GBackEnd.h"

class GDotBackEnd : public GSingleRuleBackEnd {
public:
  GDotBackEnd():_edgeLabelOutput(true) {}
  void setEdgeLabelOutput(bool);
  bool getEdgeLabelOutput();
protected:
  virtual void generateGrammarProlog(GGrammar* grammar);
  virtual void generateProductionLhs(GNonTerminal* lhs);
  virtual void generateProductionOperator();
  virtual void generateProductionRhsDelimiter();
  virtual void generateProductionRhsNonTerminal(GNonTerminal* rhsSymbol);
  virtual void generateProductionRhsTerminal(GTerminal* rhsSymbol);
  virtual void generateEquation(GEquation* equation);
  virtual void generateGrammarEpilog(GGrammar* _);
private:
  bool _edgeLabelOutput;
};

#endif
