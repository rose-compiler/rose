// Author: Markus Schordan

#ifndef GTEXTBACKEND_H
#define GTEXTBACKEND_H

#include "GBackEnd.h"



class GTextBackEnd : public GBnfBackEnd {
public:
  GTextBackEnd();

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
};

#endif
