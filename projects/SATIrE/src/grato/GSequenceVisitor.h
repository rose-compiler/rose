// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GSequenceVisitor.h,v 1.3 2008-04-30 09:53:21 gergo Exp $

// Author: Markus Schordan

#ifndef H_GSEQUENCEVISITOR
#define H_GSEQUENCEVISITOR

#include "GrammarIr.h"

class GSequenceVisitor : public GVisitor {
public:
  virtual ~GSequenceVisitor();
  virtual void visitGrammar(GGrammar*);
  virtual void visitProductionList(GProductionList*);
  virtual void visitProduction(GProduction*);
  virtual void visitProductionRhsList(GProductionRhsList*);
  virtual void visitProductionRhs(GProductionRhs*);
  virtual void visitTerminal(GTerminal*);
  virtual void visitNonTerminal(GNonTerminal*);
  list<GNode*> sequence;
  string toString();
};

class GEnhancedTokenStreamVisitor : public GBAVisitor {
public:
  virtual ~GEnhancedTokenStreamVisitor();
  virtual void beforeVisitGrammar(GGrammar*);
  virtual void afterVisitGrammar(GGrammar*);
  virtual void beforeVisitProductionList(GProductionList*);
  virtual void afterVisitProductionList(GProductionList*);
  virtual void beforeVisitProduction(GProduction*);
  virtual void afterVisitProduction(GProduction*);
  virtual void beforeVisitProductionRhsList(GProductionRhsList*);
  virtual void afterVisitProductionRhsList(GProductionRhsList*);
  virtual void beforeVisitProductionRhs(GProductionRhs*);
  virtual void afterVisitProductionRhs(GProductionRhs*);
  virtual void beforeVisitTerminal(GTerminal*);
  virtual void afterVisitTerminal(GTerminal*);
  virtual void beforeVisitNonTerminal(GNonTerminal*);
  virtual void afterVisitNonTerminal(GNonTerminal*);
  list<GEnhancedToken*> enhancedTokenStream;
  string toString();
};

#endif
