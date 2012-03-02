// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GDotBackEnd.C,v 1.2 2007-03-08 15:36:49 markus Exp $

#include "GDotBackEnd.h"

void 
GDotBackEnd::setEdgeLabelOutput(bool state) {
  _edgeLabelOutput=state;
}

bool 
GDotBackEnd::getEdgeLabelOutput() {
  return _edgeLabelOutput;
}

void
GDotBackEnd::generateGrammarProlog(GGrammar* grammar) {
  cout << "digraph G {" << endl;
}

void
GDotBackEnd::generateProductionLhs(GNonTerminal* lhs) {
  cout << lhs->getName();
}

void
GDotBackEnd::generateProductionOperator() {
  cout << " -> ";
}

void
GDotBackEnd::generateProductionRhsTerminal(GTerminal* rhsSymbol) {
  cout << rhsSymbol->getName() << "[color=blue, weight=1];" << endl;

  GSymbolList& symList=rhsSymbol->getSymbolList();
  for(GSymbolList::iterator j=symList.begin();j!=symList.end();j++) {
    cout << rhsSymbol->getName() << " -> " << (*j)->getName();
    if(getEdgeLabelOutput())
      cout << "[label=\"" <<(*j)->getAccessName() << "\", weight=0]";
    cout << ";" << endl; 
    //<< (*j)->getRegexOperatorString();
  }
}

void
GDotBackEnd::generateProductionRhsNonTerminal(GNonTerminal* rhsSymbol) {
  cout << rhsSymbol->getName() << "[color=blue];" << endl;
}

void
GDotBackEnd::generateProductionRhsDelimiter() {
}

void
GDotBackEnd::generateEquation(GEquation* equation) {
}

void
GDotBackEnd::generateGrammarEpilog(GGrammar* _) {
  cout << "}" << endl;
}

