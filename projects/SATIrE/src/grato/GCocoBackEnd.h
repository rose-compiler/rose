// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GCocoBackEnd.h,v 1.2 2007-03-08 15:36:49 markus Exp $

#ifndef GCOCOBACKEND_H
#define GCOCOBACKEND_H

#include "GBackEnd.h"



class GCocoBackEnd : public GBnfBackEnd {
public:
  GCocoBackEnd() {}

protected:
  virtual void generateGrammarProlog(GGrammar* grammar);
  //virtual void generateProduction(GNonTerminal* lhs,GProductionRhsList* rhs);
  virtual void generateProductionLhs(GNonTerminal* lhs);
  virtual void generateProductionOperator();
  virtual void generateProductionRhsListSeparator();
  virtual void generateProductionRhsListDelimiter();
  virtual void generateProductionRhsNonTerminal(GNonTerminal* _, GNonTerminal* rhsSymbol);
  virtual void generateProductionRhsTerminal(GNonTerminal* _, GTerminal* rhsSymbol);
  virtual void generateEquation(GEquation* equation);
  virtual void generateGrammarEpilog(GGrammar* grammar);
};

#endif
