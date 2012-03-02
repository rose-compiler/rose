// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GTextTransformation.C,v 1.2 2007-03-08 15:36:49 markus Exp $

// Author: Markus Schordan

#include "GTextTransformation.h"
#include "GProperty.h"

extern unsigned int transformationrulenumber;
extern string auxiliaryNonTerminalPrefix;
extern string auxiliaryNonTerminalPostfix;

GTextTransformation::~GTextTransformation() {
  delete _testGrammar;
}

void GTextTransformation::printRegisteredAuxRules() {
  // lame delete

  for(list<pair<string,GSymbol*> >::iterator i=auxRuleList.begin();
      i!=auxRuleList.end();
      i++) {
    // generate rules
    GSymbol* sym=(*i).second;
    cout << (*i).first;
    generateProductionOperator();
    if(sym->isTerminal()) {
      generateProductionRhsTerminal(0,dynamic_cast<GTerminal*>(sym));
    }
    if(sym->isNonTerminal())
      generateProductionRhsNonTerminal(0,dynamic_cast<GNonTerminal*>(sym));
    generateProductionRhsListDelimiter();
  }
}

string generateNewAuxiliarySymbolName(string rhsSym) {
  return auxiliaryNonTerminalPrefix+rhsSym+auxiliaryNonTerminalPostfix;
}

void 
GTextTransformation::registerAuxRule(string auxSymName,GSymbol* sym) { 
  //assert(dynamic_cast<GTerminal*>(sym));

  // ensure auxSymName has not been inserted yet (possibly with different sym)
  for(list<pair<string,GSymbol*> >::iterator i=auxRuleList.begin();
      i!=auxRuleList.end();
      ++i) {
    if((*i).first==auxSymName) {
      return; // don't insert duplicate
    }
  }
  auxRuleList.push_back(pair<string,GSymbol*>(auxSymName,sym));
  return; 
}

void
GTextTransformation::generateProductionRhsTerminal(GNonTerminal* _, GTerminal* rhsSymbol) {
  cout << rhsSymbol->getName();
  GSymbolList& symList=rhsSymbol->getSymbolList();
  cout << " ( ";
  for(GSymbolList::iterator j=symList.begin();j!=symList.end();j++) {
    if(j!=symList.begin()) cout << ", ";

    switch(transformationrulenumber) {
    case 0: 
      {
	  cout << (*j)->getAccessName() << ":";
	  cout << (*j)->getName() << (*j)->getRegexOperatorString();
      }
      break;
    case 1: 
      {
	/* RULE 1: 
           introduce auxiliary non-terminal and rule
        */
	GProperty property;
	property.generate(_testGrammar);
	set<string> toTransform=property.intersectionNonTerminalsAndTerminals();
	string rhsSym=(*j)->getName();
	
	if(toTransform.find(rhsSym)!=toTransform.end()) {
	  // symbol is one of those that need to be replaced with new aux-sym
	  string newName=generateNewAuxiliarySymbolName(rhsSym);
	  cout << (*j)->getAccessName() << ":"; // XXX
	  cout << newName;
	  cout << (*j)->getRegexOperatorString();

	  registerAuxRule(newName,_testGrammar->getRhsTerminal((*j)->getName()));
	} else {
	  cout << (*j)->getAccessName() << ":";
	  cout << (*j)->getName() << (*j)->getRegexOperatorString();
	}
      }
      break;
    case 2: 
      {
	/* RULE 2: 
           generate the baseType name instead of the NonTerminal 
           (which also exists as terminal) 
        */
	GProperty property;
	property.generate(_testGrammar);
	set<string> toTransform=property.intersectionNonTerminalsAndTerminals();
	string rhsSym=(*j)->getName();
	
	if(toTransform.find(rhsSym)!=toTransform.end()) {
	  // symbol is one of those that need to be replaced by base type
	  string newName=_testGrammar->getLhsOfRhsTerminal(rhsSym);
	  cout << (*j)->getAccessName() << ":"; // XXX
	  cout << newName;
	  cout << (*j)->getRegexOperatorString();
	} else {
	  cout << (*j)->getAccessName() << ":";
	  cout << (*j)->getName() << (*j)->getRegexOperatorString();
	}
      }
      break;
    } // switch
  }
  cout << " )" << endl;
}

