// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GBackEnd.C,v 1.3 2008-08-23 13:46:56 gergo Exp $


#include "GBackEnd.h"

GBackEnd::GBackEnd() {
}

GBackEnd::~GBackEnd() {
}

void
GBackEnd::generate(GGrammar* grammar) {
  generateGrammarProlog(grammar);
  GProductionList* pl=grammar->getProductionList();
  assert(pl!=0);
  for(GProductionList::iterator i=pl->begin();i!=pl->end();i++) {
    GNonTerminal* lhs=(*i)->getLhs();
    GProductionRhsList* rhs=(*i)->getRhs();
    generateProduction(lhs,rhs);
  }
  GEquationList* el=grammar->getEquationList();
  assert(el!=0);
  for(GEquationList::iterator i=el->begin();i!=el->end();i++) {
    generateEquation(*i);
  }
  generateGrammarEpilog(grammar);
}

void GBackEnd::generateGrammarProlog(GGrammar* grammar) {
}
void GBackEnd::generateEquationListProlog() {
}
void GBackEnd::generateEquation(GEquation* equation) {
}
void GBackEnd::generateEquationListEpilog() {
}
void GBackEnd::generateGrammarEpilog(GGrammar* grammar) {
}

GBnfBackEnd::GBnfBackEnd() {
}

GBnfBackEnd::~GBnfBackEnd() {
}

void
GBnfBackEnd::generateProduction(GNonTerminal* lhs,GProductionRhsList* rhs) {
  generateProductionLhs(lhs);
  generateProductionOperator();
  for(GProductionRhsList::iterator i=rhs->begin();i!=rhs->end();i++) {
    if(i!=rhs->begin()) generateProductionRhsListSeparator();
    GSymbol* rhsSymbol=(*i)->getRhs();
    //generateSymbol(rhsSymbol);
    if(rhsSymbol->isTerminal()) {
      generateProductionRhsTerminal(lhs,dynamic_cast<GTerminal*>(rhsSymbol));
    } else {
      generateProductionRhsNonTerminal(lhs,dynamic_cast<GNonTerminal*>(rhsSymbol));
    }
  }
  generateProductionRhsListDelimiter();
}

GSingleRuleBackEnd::GSingleRuleBackEnd() {
}

GSingleRuleBackEnd::~GSingleRuleBackEnd() {
}

void
GSingleRuleBackEnd::generateProduction(GNonTerminal* lhs,GProductionRhsList* rhs) {
  for(GProductionRhsList::iterator i=rhs->begin();i!=rhs->end();i++) {
    generateProductionLhs(lhs);
    generateProductionOperator();
    GSymbol* rhsSymbol=(*i)->getRhs();
    //generateSymbol(rhsSymbol);
    if(rhsSymbol->isTerminal()) {
      generateProductionRhsTerminal(dynamic_cast<GTerminal*>(rhsSymbol));
    } else {
      generateProductionRhsNonTerminal(dynamic_cast<GNonTerminal*>(rhsSymbol));
    }
    generateProductionRhsDelimiter();
  }
}

