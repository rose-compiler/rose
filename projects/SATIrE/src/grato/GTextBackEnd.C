// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GTextBackEnd.C,v 1.2 2007-03-08 15:36:49 markus Exp $

// Author: Markus Schordan

#include "GTextBackEnd.h"

extern unsigned int transformationrulenumber;

GTextBackEnd::GTextBackEnd() {
}

void
GTextBackEnd::generateGrammarProlog(GGrammar* grammar) {
  cout << "SYNTAX" << endl;
  cout << "START:" << grammar->getStartSymbol()->getName() << endl;
}

void
GTextBackEnd::generateProductionLhs(GNonTerminal* lhs) {
  cout << lhs->getName();
}

void
GTextBackEnd::generateProductionOperator() {
  cout << " : ";
}

void
GTextBackEnd::generateProductionRhsListSeparator() {
  cout << " | ";
}

void
GTextBackEnd::generateProductionRhsTerminal(GNonTerminal* _, GTerminal* rhsSymbol) {
  cout << rhsSymbol->getName();
  GSymbolList& symList=rhsSymbol->getSymbolList();
  cout << " ( ";
  for(GSymbolList::iterator j=symList.begin();j!=symList.end();j++) {
    if(j!=symList.begin()) 
      cout << ", ";
    cout << (*j)->getAccessName() << ":";
    cout << (*j)->getName() << (*j)->getRegexOperatorString();
  }
  cout << " )" << endl;
}

void
GTextBackEnd::generateProductionRhsNonTerminal(GNonTerminal* _, GNonTerminal* rhsSymbol) {
    cout <<  rhsSymbol->getName() << endl;
}

void
GTextBackEnd::generateProductionRhsListDelimiter() {
  cout << " ;" << endl << endl;
}

void
GTextBackEnd::generateEquation(GEquation* equation) {
  cout << equation->getLhs() << " == " << equation->getRhs() << ";" << endl;
}
