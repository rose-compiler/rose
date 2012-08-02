// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GrammarIr.C,v 1.3 2008-08-23 13:46:57 gergo Exp $

/*************************************************************
* Author: Markus Schordan, 2005                              *
*************************************************************/

#include "GrammarIr.h"

string
GEquation::getLhs() { 
  return _lhs; 
}
string 
GEquation::getRhs() { 
  return _rhs; 
}

string 
GSymbol::getAccessName() {
  return _accessName; 
}

string 
GSymbol::getName() {
  return _name; 
}

RegexOperator 
GSymbol::getRegexOperator() {
  return _regexop;
}

string
GSymbol::getRegexOperatorString() {
  switch(_regexop) {
  case REGEX_ONE: return "";
  case REGEX_PLUS: return "+";
  case REGEX_STAR: return "*";
  case REGEX_OPT: return "?";
  default: assert(false);
  }
}

void
GSymbol::setRegexOperator(RegexOperator regexop) {
  _regexop=regexop;
}

GSymbol::GArity
GSymbol::arity() { 
  return _symbolList.size(); 
}

GSymbolList& 
GSymbol::getSymbolList() { 
  return _symbolList; 
}

GNonTerminal*
GProduction::getLhs() {
  return _lhs;
}

GProductionRhsList*
GProduction::getRhs() {
  return _rhs;
}

GProductionList*
GGrammar::getProductionList() {
  return _productionList;
}

GEquationList* 
GGrammar::getEquationList() { 
  return _equationList; 
}

GNonTerminal* 
GGrammar::getStartSymbol() { 
  return _startSymbol; 
}

unsigned int 
GGrammar::productionsNum() {
  unsigned int num=0;
  GProductionList l= *getProductionList();
  for(GProductionList::iterator i=l.begin(); i!=l.end();i++) {
    num += (*i)->getRhs()->size();
  }
  return num;
}
unsigned int 
GGrammar::bnfProductionsNum() {
  return getProductionList()->size();
}

string 
GGrammar::getLhsOfRhsTerminal(string terminalName) {
  for(GProductionList::iterator i=getProductionList()->begin();
      i!=getProductionList()->end();
      i++) {
    GProduction* production=*i;
    GProductionRhsList* rhslist=production->getRhs();
    for(GProductionRhsList::iterator j=rhslist->begin();
	j!=rhslist->end();
	j++) {
      GSymbol* rhs=(*j)->getRhs();
      if(rhs->isTerminal() && rhs->getName()==terminalName)
	return (production->getLhs())->getName();
    }
  }
  return ""; // empty lhs
}

GTerminal* 
GGrammar::getRhsTerminal(string terminal) {
  for(GProductionList::iterator i=getProductionList()->begin();
      i!=getProductionList()->end();
      i++) {
    GProduction* production=*i;
    GProductionRhsList* rhslist=production->getRhs();
    for(GProductionRhsList::iterator j=rhslist->begin();
	j!=rhslist->end();
	j++) {
      GSymbol* rhs=(*j)->getRhs();
      if(rhs->isTerminal() && rhs->getName()==terminal)
	return dynamic_cast<GTerminal*>(rhs);
    }
  }
  return 0; // no terminal on rhs
}


GProductionRhsList* 
GGrammar::getRhsOfLhs(string lhsNonTerminalName) {
  for(GProductionList::iterator i=getProductionList()->begin();
      i!=getProductionList()->end();
      i++) {
    GProduction* production=*i;
    GSymbol* lhs=production->getLhs();
    if(lhs->getName()==lhsNonTerminalName) {
      assert(lhs->isNonTerminal());
      return (*i)->getRhs();
    }
  }
  return 0; // no rhs found for lhs nonterminal name
}

unsigned int
GProductionRhsList::size() {
  return productionRhsList.size();
}

// ------------------------
// Visitor accept functions
// ------------------------
void
GGrammar::accept(GVisitor& visitor) {
  visitor.visitGrammar(this);
  getProductionList()->accept(visitor);
  getStartSymbol()->accept(visitor);
}

void
GNonTerminal::accept(GVisitor& visitor) {
  visitor.visitNonTerminal(this);
}

void
GTerminal::accept(GVisitor& visitor) {
  visitor.visitTerminal(this);
  for(GSymbolList::iterator i=_symbolList.begin(); i!=_symbolList.end();i++) {
    (*i)->accept(visitor);
  }
}

unsigned int GProductionList::size() {
  return productionList.size();
}

void
GProductionList::accept(GVisitor& visitor) {
  visitor.visitProductionList(this);
  for(GProductionList::iterator i=productionList.begin();i!=productionList.end();i++) {
    (*i)->accept(visitor);
  }
}

void
GProduction::accept(GVisitor& visitor) {
  visitor.visitProduction(this);
  _lhs->accept(visitor);
  _rhs->accept(visitor);
}

void
GProductionRhsList::accept(GVisitor& visitor) {
  visitor.visitProductionRhsList(this);
  for(GProductionRhsList::iterator i=productionRhsList.begin();i!=productionRhsList.end();i++) {
    (*i)->accept(visitor);
  }
}

void
GProductionRhs::accept(GVisitor& visitor) {
  visitor.visitProductionRhs(this);
  _symbol->accept(visitor);
}

// ------------------------
// BA Visitor accept functions
// ------------------------
void
GGrammar::baaccept(GBAVisitor& visitor) {
  visitor.beforeVisitGrammar(this);
  getProductionList()->baaccept(visitor);
  getStartSymbol()->baaccept(visitor);
  visitor.afterVisitGrammar(this);
}

void
GNonTerminal::baaccept(GBAVisitor& visitor) {
  visitor.beforeVisitNonTerminal(this);
  visitor.afterVisitNonTerminal(this);
}

void
GTerminal::baaccept(GBAVisitor& visitor) {
  visitor.beforeVisitTerminal(this);
  for(GSymbolList::iterator i=_symbolList.begin(); i!=_symbolList.end();i++) {
    (*i)->baaccept(visitor);
  }
  visitor.afterVisitTerminal(this);
}

void
GProductionList::baaccept(GBAVisitor& visitor) {
  visitor.beforeVisitProductionList(this);
  for(GProductionList::iterator i=productionList.begin();i!=productionList.end();i++) {
    (*i)->baaccept(visitor);
  }
  visitor.afterVisitProductionList(this);
}

void
GProduction::baaccept(GBAVisitor& visitor) {
  visitor.beforeVisitProduction(this);
  _lhs->baaccept(visitor);
  _rhs->baaccept(visitor);
  visitor.afterVisitProduction(this);
}

void
GProductionRhsList::baaccept(GBAVisitor& visitor) {
  visitor.beforeVisitProductionRhsList(this);
  for(GProductionRhsList::iterator i=productionRhsList.begin();i!=productionRhsList.end();i++) {
    (*i)->baaccept(visitor);
  }
  visitor.afterVisitProductionRhsList(this);
}

void
GProductionRhs::baaccept(GBAVisitor& visitor) {
  visitor.beforeVisitProductionRhs(this);
  _symbol->baaccept(visitor);
  visitor.afterVisitProductionRhs(this);
}

// ------------------------
// Visitor default visit functions
// ------------------------

void GVisitor::visitGrammar(GGrammar* grammar) {
  std::cout << "Visiting Grammar" << endl; 
}
void GVisitor::visitProductionList(GProductionList* productionList) {
  std::cout << "Visiting ProductionList" << endl; 
}
void GVisitor::visitProduction(GProduction* production) {
  std::cout << "Visiting Production" << endl; 
}
void GVisitor::visitProductionRhsList(GProductionRhsList* productionRhsList) { 
  std::cout << "Visiting ProductionRhsList" << endl; 
}
void GVisitor::visitProductionRhs(GProductionRhs* productionRhs) { 
  std::cout << "Visiting ProductionRhs" << endl; 
}
void GVisitor::visitNonTerminal(GNonTerminal* nonTerminal) {
  std::cout << "Visiting NonTerminal" << endl; 
}
void GVisitor::visitTerminal(GTerminal* terminal) {
  std::cout << "Visiting Terminal" << endl; 
}


// ------------------------
// BA Visitor default visit functions
// ------------------------

void GBAVisitor::beforeVisitGrammar(GGrammar* grammar) {
  std::cout << "bVisiting Grammar" << endl; 
}
void GBAVisitor::afterVisitGrammar(GGrammar* grammar) {
  std::cout << "aVisiting Grammar" << endl; 
}
void GBAVisitor::beforeVisitProductionList(GProductionList* productionList) {
  std::cout << "bVisiting ProductionList" << endl; 
}
void GBAVisitor::afterVisitProductionList(GProductionList* productionList) {
  std::cout << "aVisiting ProductionList" << endl; 
}
void GBAVisitor::beforeVisitProduction(GProduction* production) {
  std::cout << "bVisiting Production" << endl; 
}
void GBAVisitor::afterVisitProduction(GProduction* production) {
  std::cout << "aVisiting Production" << endl; 
}
void GBAVisitor::beforeVisitProductionRhsList(GProductionRhsList* productionRhsList) { 
  std::cout << "bVisiting ProductionRhsList" << endl; 
}
void GBAVisitor::afterVisitProductionRhsList(GProductionRhsList* productionRhsList) { 
  std::cout << "aVisiting ProductionRhsList" << endl; 
}
void GBAVisitor::beforeVisitProductionRhs(GProductionRhs* productionRhs) { 
  std::cout << "bVisiting ProductionRhs" << endl; 
}
void GBAVisitor::afterVisitProductionRhs(GProductionRhs* productionRhs) { 
  std::cout << "aVisiting ProductionRhs" << endl; 
}
void GBAVisitor::beforeVisitNonTerminal(GNonTerminal* nonTerminal) {
  std::cout << "bVisiting NonTerminal" << endl; 
}
void GBAVisitor::afterVisitNonTerminal(GNonTerminal* nonTerminal) {
  std::cout << "aVisiting NonTerminal" << endl; 
}
void GBAVisitor::beforeVisitTerminal(GTerminal* terminal) {
  std::cout << "bVisiting Terminal" << endl; 
}
void GBAVisitor::afterVisitTerminal(GTerminal* terminal) {
  std::cout << "aVisiting Terminal" << endl; 
}

// ------------------------
// Visitor default destructors
// ------------------------
GVisitor::~GVisitor() {}
GBAVisitor::~GBAVisitor() {}

// ------------------------
// Grammar variant functions
// ------------------------

EVariant GGrammar::variant() { return VGGrammar; }
EVariant GProductionList::variant() { return VGProductionList; }
EVariant GProduction::variant() { return VGProduction; }
EVariant GProductionRhsList::variant() { return VGProductionRhsList; }
EVariant GProductionRhs::variant() { return VGProductionRhs; }
EVariant GTerminal::variant() { return VGTerminal; }
EVariant GNonTerminal::variant() { return VGNonTerminal; }

// Grammar virtual destructors

GNode::~GNode() {}
GSymbol::~GSymbol() {}
GTerminal::~GTerminal() {}
GNonTerminal::~GNonTerminal() {}
GProductionRhs::~GProductionRhs() { delete _symbol; }
GProductionList::~GProductionList() {
  DELETE_LIST_PTROBJECTS(productionList,GProduction);
}
GProductionRhsList::~GProductionRhsList() {
  DELETE_LIST_PTROBJECTS(productionRhsList,GProductionRhs);
}
GProduction::~GProduction() { delete _lhs; delete _rhs; }
GGrammar::~GGrammar() { delete _productionList; delete _equationList; delete _startSymbol; }

void GTransformation::eliminateNonTerminal(GGrammar* g,GNonTerminal* sym) {
}
void GTransformation::eliminateProduction(GGrammar* g, GProduction* p) {
}
void GTransformation::deleteSymbol(GGrammar* g,GSymbol* sym) {
}
void GTransformation::deleteProduction(GGrammar* g, GProduction* p) {
}
void GTransformation::addProduction(GGrammar* g, GProduction* p) {
}
void GTransformation::addSymbolToSymbol(GGrammar* g, GSymbol* sym, GSymbol* newsym, unsigned int position) {
}
GProductionList* GTransformation::findEmptyProductions(GGrammar* g) { 
  return new GProductionList();
}
unsigned int GTransformation::productionsNum(GGrammar* g) {
  return g->productionsNum();
}
unsigned int GTransformation::bnfProductionsNum(GGrammar* g) {
  return g->bnfProductionsNum();
}

bool GTransformation::symbolsHaveSameName(GSymbol* s1, GSymbol* s2) {
  return s1->getName()==s2->getName();
}
