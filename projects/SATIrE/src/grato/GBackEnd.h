#ifndef GBACKEND_H
#define GBACKEND_H

#include "GrammarIr.h"

class GBackEnd {
public:
  GBackEnd();
  void generate(GGrammar* grammar);
protected:
  virtual void generateGrammarProlog(GGrammar* grammar);
  virtual void generateProduction(GNonTerminal* lhs,GProductionRhsList* rhs)=0;

  virtual void generateProductionLhs(GNonTerminal* lhs){}
  virtual void generateProductionOperator(){}

  virtual void generateEquationListProlog();
  virtual void generateEquation(GEquation* equation);
  virtual void generateEquationListEpilog();
  virtual void generateGrammarEpilog(GGrammar* grammar);

private:
};

class GBnfBackEnd : public GBackEnd {
public:
  GBnfBackEnd();
protected:
  virtual void generateProduction(GNonTerminal* lhs,GProductionRhsList* rhs);
  virtual void generateProductionRhsListSeparator(){}
  virtual void generateProductionRhsListDelimiter(){}
  virtual void generateProductionRhsNonTerminal(GNonTerminal* lhs, GNonTerminal* rhsSymbol){}
  virtual void generateProductionRhsTerminal(GNonTerminal* lhs, GTerminal* rhsSymbol){}
};

class GSingleRuleBackEnd : public GBackEnd {
public:
  GSingleRuleBackEnd();
protected:
  virtual void generateProduction(GNonTerminal* lhs,GProductionRhsList* rhs);
  virtual void generateProductionRhsDelimiter(){}
  virtual void generateProductionRhsNonTerminal(GNonTerminal* rhsSymbol){}
  virtual void generateProductionRhsTerminal(GTerminal* rhsSymbol){}
};

#endif
