// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GSequenceVisitor.C,v 1.2 2007-03-08 15:36:49 markus Exp $

// Author: Markus Schordan

#include "GSequenceVisitor.h"
#include <sstream>

GSequenceVisitor::~GSequenceVisitor() {
  for(std::list<GNode*>::iterator i=sequence.begin(); i!=sequence.end(); ++i) {
    delete *i;
  }
}

GEnhancedTokenStreamVisitor::~GEnhancedTokenStreamVisitor() {
  for(std::list<GEnhancedToken*>::iterator i=enhancedTokenStream.begin(); i!=enhancedTokenStream.end(); ++i) {
    delete *i;
  }
}

void GSequenceVisitor::visitGrammar(GGrammar* n){ sequence.push_back(n); }
void GSequenceVisitor::visitProductionList(GProductionList* n){ sequence.push_back(n); }
void GSequenceVisitor::visitProduction(GProduction* n){ sequence.push_back(n); }
void GSequenceVisitor::visitProductionRhsList(GProductionRhsList* n){ sequence.push_back(n); }
void GSequenceVisitor::visitProductionRhs(GProductionRhs* n){ sequence.push_back(n); }
void GSequenceVisitor::visitTerminal(GTerminal* n){ sequence.push_back(n); }
void GSequenceVisitor::visitNonTerminal(GNonTerminal* n){ sequence.push_back(n); }

string GSequenceVisitor::toString() {
  std::stringstream ss;
  for(std::list<GNode*>::iterator i=sequence.begin();i!=sequence.end();i++) {
    ss << "Token:" << typeid(*i).name() << " Variant:" << (*i)->variant() << "\n";
  }
  return ss.str();
}

void GEnhancedTokenStreamVisitor::beforeVisitGrammar(GGrammar* n){ 
  enhancedTokenStream.push_back(new GEnhancedToken(n)); 
  enhancedTokenStream.push_back(new GEnhancedToken(LB)); 
}
void GEnhancedTokenStreamVisitor::afterVisitGrammar(GGrammar* n){
  enhancedTokenStream.push_back(new GEnhancedToken(RB)); 
}
void GEnhancedTokenStreamVisitor::beforeVisitProductionList(GProductionList* n){
  enhancedTokenStream.push_back(new GEnhancedToken(n));
  enhancedTokenStream.push_back(new GEnhancedToken(LB)); 
 }
void GEnhancedTokenStreamVisitor::afterVisitProductionList(GProductionList* n){
  enhancedTokenStream.push_back(new GEnhancedToken(RB)); 
}
void GEnhancedTokenStreamVisitor::beforeVisitProduction(GProduction* n){ 
  enhancedTokenStream.push_back(new GEnhancedToken(n)); 
  enhancedTokenStream.push_back(new GEnhancedToken(LB)); 
}
void GEnhancedTokenStreamVisitor::afterVisitProduction(GProduction* n){
  enhancedTokenStream.push_back(new GEnhancedToken(RB)); 
}
void GEnhancedTokenStreamVisitor::beforeVisitProductionRhsList(GProductionRhsList* n){ 
  enhancedTokenStream.push_back(new GEnhancedToken(n));
  enhancedTokenStream.push_back(new GEnhancedToken(LB)); 
 }
void GEnhancedTokenStreamVisitor::afterVisitProductionRhsList(GProductionRhsList* n){
  enhancedTokenStream.push_back(new GEnhancedToken(RB)); 
}
void GEnhancedTokenStreamVisitor::beforeVisitProductionRhs(GProductionRhs* n){
  enhancedTokenStream.push_back(new GEnhancedToken(n)); 
  enhancedTokenStream.push_back(new GEnhancedToken(LB)); 
}
void GEnhancedTokenStreamVisitor::afterVisitProductionRhs(GProductionRhs* n){
  enhancedTokenStream.push_back(new GEnhancedToken(RB)); 
}
void GEnhancedTokenStreamVisitor::beforeVisitTerminal(GTerminal* n){ 
  enhancedTokenStream.push_back(new GEnhancedToken(n)); 
  enhancedTokenStream.push_back(new GEnhancedToken(LB)); 
}
void GEnhancedTokenStreamVisitor::afterVisitTerminal(GTerminal* n){
  enhancedTokenStream.push_back(new GEnhancedToken(RB)); 
}
void GEnhancedTokenStreamVisitor::beforeVisitNonTerminal(GNonTerminal* n){ 
  enhancedTokenStream.push_back(new GEnhancedToken(n)); 
  enhancedTokenStream.push_back(new GEnhancedToken(LB)); 
}
void GEnhancedTokenStreamVisitor::afterVisitNonTerminal(GNonTerminal* n){
  enhancedTokenStream.push_back(new GEnhancedToken(RB)); 
}

string GEnhancedTokenStreamVisitor::toString() {
  std::stringstream ss;
  for(std::list<GEnhancedToken*>::iterator i=enhancedTokenStream.begin();
      i!=enhancedTokenStream.end();
      i++) {
    ss << "Enhanced Token:" << (*i)->toString() << "\n";
  }
  return ss.str();
}
