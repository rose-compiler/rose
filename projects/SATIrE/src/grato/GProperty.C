// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GProperty.C,v 1.2 2007-03-08 15:36:49 markus Exp $

#include "GProperty.h"

GProperty::~GProperty() {}

void
GProperty::generateGrammarProlog(GGrammar* grammar) {
}

void
GProperty::generateProductionLhs(GNonTerminal* lhs) {
  nonTerminals.insert(lhs->getName());
}

void
GProperty::generateProductionOperator() {
}

void
GProperty::generateProductionRhsListSeparator() {
}

void
GProperty::generateProductionRhsTerminal(GNonTerminal* _, GTerminal* rhsSymbol) {
  terminals.insert(rhsSymbol->getName());
  GSymbolList& symList=rhsSymbol->getSymbolList();
  for(GSymbolList::iterator j=symList.begin();j!=symList.end();j++) {
    nonTerminals.insert((*j)->getName());
    childrenNonTerminals.insert((*j)->getName());
  }
  rhsNum++; // for statistics
}

void
GProperty::generateProductionRhsNonTerminal(GNonTerminal* _, GNonTerminal* rhsSymbol) {
  nonTerminals.insert(rhsSymbol->getName());
  rhsNum++; // for statistics
}

void
GProperty::generateProductionRhsListDelimiter() {
}

void
GProperty::generateEquation(GEquation* equation) {
  //cout << equation->getLhs() << " == " << equation->getRhs() << ";" << endl;
}
