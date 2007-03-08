// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GCocoBackEnd.C,v 1.2 2007-03-08 15:36:49 markus Exp $

#include "GCocoBackEnd.h"
#include "GProperty.h"

extern unsigned int transformationrulenumber;

void
GCocoBackEnd::generateGrammarProlog(GGrammar* grammar) {
  cout << "$CX /* Generate C++ code */" << endl;
  cout << "COMPILER " << grammar->getStartSymbol()->getName() << endl;
  cout << endl << "TOKENS" << endl << endl;
  GProperty prop;
  prop.generate(grammar);
  for(set<std::string>::iterator i=prop.terminals.begin();
      i!=prop.terminals.end();
      i++) {
    std::cout << *i << std::endl;
  }
}

void
GCocoBackEnd::generateGrammarEpilog(GGrammar* grammar) {
  cout << "END " << grammar->getStartSymbol()->getName() << "." << endl;
}

void
GCocoBackEnd::generateProductionLhs(GNonTerminal* lhs) {
  cout << lhs->getName();
}

void
GCocoBackEnd::generateProductionOperator() {
  cout << " = ";
}

void
GCocoBackEnd::generateProductionRhsListSeparator() {
  cout << " | ";
}

void
GCocoBackEnd::generateProductionRhsTerminal(GNonTerminal* _, GTerminal* rhsSymbol) {
  cout << rhsSymbol->getName();
  GSymbolList& symList=rhsSymbol->getSymbolList();
  cout << " \"(\" ";
  for(GSymbolList::iterator j=symList.begin();j!=symList.end();j++) {
    if(j!=symList.begin()) 
      cout << " \",\" "; // no comma (insert comma here if necessary)
    //cout << (*j)->getAccessName() << ":"; NO access names with Coco
    RegexOperator regOp=(*j)->getRegexOperator();
    switch(regOp) {
    case REGEX_ONE: cout << (*j)->getName();break;
    case REGEX_OPT: cout << "[" << (*j)->getName() << "]";break;
    case REGEX_PLUS: cout << (*j)->getName() << /*" \",\"*/" {" << (*j)->getName() <<"}" ;break;
    case REGEX_STAR: cout << "{" << (*j)->getName() << "}";break;
    default: cout << "<" << (*j)->getName() << ":UNKNOWN REGEX> ";
    }
  }
  cout << " \")\"" << endl;
}

void
GCocoBackEnd::generateProductionRhsNonTerminal(GNonTerminal* _, GNonTerminal* rhsSymbol) {
    cout <<  rhsSymbol->getName() << endl;
}

void
GCocoBackEnd::generateProductionRhsListDelimiter() {
  cout << " ." << endl << endl;
}

void
GCocoBackEnd::generateEquation(GEquation* equation) {
  //cout << equation->getLhs() << " == " << equation->getRhs() << ";" << endl;
}
