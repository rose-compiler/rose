// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GTextTransformation.h,v 1.2 2007-03-08 15:36:49 markus Exp $

// Author: Markus Schordan

#ifndef GTEXTTRANSFORM_H
#define GTEXTTRANSFORM_H

#include "GTextBackEnd.h"

class GTextTransformation : public GTextBackEnd {
public:
  GTextTransformation() {}
  virtual ~GTextTransformation();
  void transform(GGrammar* grammar) {
    _testGrammar=grammar;
    generate(grammar);
    printRegisteredAuxRules(); // postprocessing of registered auxiliary rules for generation
  }

protected:
  //virtual void generateGrammarProlog(GGrammar* grammar);
  virtual void generateProductionRhsTerminal(GNonTerminal* _, GTerminal* rhsSymbol);

protected:
  void printRegisteredAuxRules();
  void registerAuxRule(string auxSymName,GSymbol* sym);
  list<pair<string,GSymbol*> > auxRuleList;

private:
  GGrammar* _testGrammar;
};

#endif
