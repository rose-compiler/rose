// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GProperty.h,v 1.2 2007-03-08 15:36:49 markus Exp $

#ifndef GPROPERTY_H
#define GPROPERTY_H

#include "GBackEnd.h"
#include <set>
#include "setalgorithms.h"

class GProperty : public GBnfBackEnd {
public:
  GProperty():rhsNum(0) {}
  virtual ~GProperty();

  set<string> terminals;
  set<string> nonTerminals;
  set<string> childrenNonTerminals;

  
  // requires: generate()
  void printSet(const set<string>& s) {
    if(s.size()==0) {
      cout << "empty";
    } else {
      for(set<string>::const_iterator i=s.begin();i!=s.end();i++) {
	cout << *i << ", ";
      }
    }
    cout << endl;
  }

  // requires: generate()
  set<string> intersectionNonTerminalsAndTerminals() {
    set<string> tmp;
    intersection(nonTerminals,terminals,tmp);
    return tmp;
  }

  // requires: generate()
  set<string> unusedChildrenNonTerminals() {
    set<string> tmp;
    set_difference(nonTerminals,childrenNonTerminals,tmp);
    return tmp;
  }

  // requires: generate()
  void printStatistics() {
    cout << "Number of Terminals   : " << terminals.size() << endl;
    cout << "Number of NonTerminals: " << nonTerminals.size() << endl;
    cout << "Number of unused CNTs : " << nonTerminals.size()-childrenNonTerminals.size() << endl;
    cout << "Number of Rules       : " << rhsNum << endl;
  }


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
private:
  unsigned int rhsNum;
};

#endif
