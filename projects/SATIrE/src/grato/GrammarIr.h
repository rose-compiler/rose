// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: GrammarIr.h,v 1.3 2008-08-23 13:46:57 gergo Exp $

/**************************************************************
* Author: Markus Schordan, 2005                               *
***************************************************************/

#ifndef GRAMMARIR_H
#define GRAMMARIR_H

#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <cassert>
#include <typeinfo>

using namespace std;

class GNode;
class GGrammar;
class GProductionList;
class GProduction;
class GProductionRhsList;
class GProductionRhs;
class GSymbol;
class GTerminal;
class GNonTerminal;

class GTransformation;
class GVisitor;

#define DELETE_LIST_PTROBJECTS(LISTNAME,OBJTYPENAME) for(std::list<OBJTYPENAME*>::iterator i=LISTNAME.begin(); i!=LISTNAME.end(); ++i) {delete *i;}

enum EVariant { VGGrammar,VGProductionList,VGProduction, VGProductionRhsList,VGProductionRhs,VGTerminal,VGNonTerminal,LB,RB,COMMA };
enum RegexOperator { REGEX_ONE, REGEX_OPT, REGEX_PLUS, REGEX_STAR };
class GSymbol;
typedef list<GSymbol*> GSymbolList;
class GEquation;
typedef list<GEquation*> GEquationList;

class GVisitor {
public:
  virtual void visitGrammar(GGrammar*);
  virtual void visitProductionList(GProductionList*);
  virtual void visitProduction(GProduction*);
  virtual void visitProductionRhsList(GProductionRhsList*);
  virtual void visitProductionRhs(GProductionRhs*);
  virtual void visitTerminal(GTerminal*);
  virtual void visitNonTerminal(GNonTerminal*);
  virtual ~GVisitor();
};

class GBAVisitor {
public:
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
  virtual ~GBAVisitor();
};

class GEquation {
public:
  GEquation(string lhs, string rhs):_lhs(lhs),_rhs(rhs) {}
  string getLhs();
  string getRhs();
private:
  string _lhs;
  string _rhs;
};

class GNode {
public:
  virtual void accept(GVisitor& visitor)=0;
  virtual void baaccept(GBAVisitor& visitor)=0;
  virtual EVariant variant()=0;
  virtual ~GNode();
};

class GEnhancedToken {
public:
  GEnhancedToken(GNode* n):node(n),variant(n->variant()){}
  GEnhancedToken(EVariant v):node(0),variant(v){}
  GNode* node;
  EVariant variant;
  std::string toString() { std::stringstream ss; ss << "("<<node<<","<<variant<<")"; return ss.str();}
};

class GFixedArityNode : public GNode {
};

class GArbitraryArityNode : public GNode {
};

class GSymbol : public GFixedArityNode {
public:
  typedef unsigned int GArity;
  GSymbol(string name):_accessName(""),_name(name),_regexop(REGEX_ONE) {}
  GSymbol(string name,RegexOperator regexOperator):_accessName(""),_name(name),_regexop(regexOperator) {}
  GSymbol(string accessName, string name, RegexOperator regexOperator):_accessName(accessName),_name(name),_regexop(regexOperator) {}
  virtual string getName();
  virtual string getAccessName();
  RegexOperator getRegexOperator();
  string getRegexOperatorString();
  void setRegexOperator(RegexOperator regexop);
  GArity arity();
  GSymbolList& getSymbolList();
  virtual bool isTerminal()=0;
  virtual bool isNonTerminal()=0;
  virtual ~GSymbol();
protected:
  GSymbolList _symbolList;
private:
  string _accessName;
  string _name;
  RegexOperator _regexop;
};

class GTerminal : public GSymbol {
public:
  GTerminal(string name):GSymbol(name) {}
  GTerminal(string name,RegexOperator regexOperator):GSymbol(name,regexOperator) {}
  GTerminal(string name,GSymbolList* symbolList):GSymbol(name,REGEX_ONE) {
    _symbolList=*symbolList;
  }
  bool isTerminal() { return true; }
  bool isNonTerminal() { return false; }
  // Visitor
  virtual void accept(GVisitor& visitor);
  virtual void baaccept(GBAVisitor& visitor);
  virtual EVariant variant();
  virtual ~GTerminal();
private:
};

class GNonTerminal : public GSymbol {
public:
  GNonTerminal(string name):GSymbol(name) {}
  GNonTerminal(string name,RegexOperator regexOperator):GSymbol(name,regexOperator) {}
  GNonTerminal(string accessName, string name,RegexOperator regexOperator):GSymbol(accessName,name,regexOperator) {}
  bool isTerminal() { return false; }
  bool isNonTerminal() { return true; }
  // Visitor
  virtual void accept(GVisitor& visitor);
  virtual void baaccept(GBAVisitor& visitor);
  virtual EVariant variant();
  virtual ~GNonTerminal();
};

class GProductionList : public GArbitraryArityNode {
public:
  GProductionList() {}
  typedef list<GProduction*>::iterator iterator;
  iterator begin() { return productionList.begin();}
  iterator end() { return productionList.end();}
  void push_back(GProduction* p) { productionList.push_back(p);}
  void remove(GProduction* p) { productionList.remove(p); }
  // Visitor
  virtual void accept(GVisitor& visitor);
  virtual void baaccept(GBAVisitor& visitor);
  virtual EVariant variant();
  virtual ~GProductionList();
  unsigned int size();
private:
  list<GProduction*> productionList;
};

class GProductionRhs : public GFixedArityNode {
public:
  GProductionRhs(GSymbol* symbol):_symbol(symbol){}
  virtual ~GProductionRhs();
  GSymbol* getRhs() { return _symbol; }
  // Visitor
  virtual void accept(GVisitor& visitor);
  virtual void baaccept(GBAVisitor& visitor);
  virtual EVariant variant();
protected:
  GSymbol* _symbol;
};

class GProductionRhsList : public GArbitraryArityNode {
public:
  GProductionRhsList(GProductionRhs* rhs) {
    this->push_back(rhs);
  }
  virtual ~GProductionRhsList();
  typedef list<GProductionRhs*>::iterator iterator;
  iterator begin() { return productionRhsList.begin();}
  iterator end() { return productionRhsList.end();}
  void push_back(GProductionRhs* p) { productionRhsList.push_back(p);}
  void remove(GProductionRhs* p) { productionRhsList.remove(p); }
  // Visitor
  virtual void accept(GVisitor& visitor);
  virtual void baaccept(GBAVisitor& visitor);
  virtual EVariant variant();
  unsigned int size();
private:
  list<GProductionRhs*> productionRhsList;
};

//!
class GProduction : public GFixedArityNode {
public:
  GProduction(GNonTerminal* nt, GSymbol* sym):_lhs(nt) { _rhs=new GProductionRhsList(new GProductionRhs(sym)); }
  GProduction(GNonTerminal* nt, GProductionRhs* rhs):_lhs(nt) { _rhs=new GProductionRhsList(rhs); }
  GProduction(GNonTerminal* nt, GProductionRhsList* l):_lhs(nt),_rhs(l) {}
  virtual ~GProduction();
  GNonTerminal* getLhs();
  GProductionRhsList* getRhs();
  // Visitor
  virtual void accept(GVisitor& visitor);
  virtual void baaccept(GBAVisitor& visitor);
  virtual EVariant variant();
private:
  GNonTerminal* _lhs;
  GProductionRhsList* _rhs;
};

class GGrammar : public GFixedArityNode {
public:
  GGrammar(GProductionList* p, GNonTerminal* start):_productionList(p),_startSymbol(start) {}
  GGrammar(GProductionList* p, GEquationList* e,GNonTerminal* start):_productionList(p),_equationList(e),_startSymbol(start) {}
  virtual ~GGrammar();
  unsigned int productionsNum();
  unsigned int bnfProductionsNum();
  GProductionList* getProductionList();
  GEquationList* getEquationList();
  GNonTerminal* getStartSymbol();

  // Visitor
  virtual void accept(GVisitor& visitor);
  virtual void baaccept(GBAVisitor& visitor);
  virtual EVariant variant();

  string getLhsOfRhsTerminal(string terminalName);

  GTerminal* getRhsTerminal(string terminal);
  GProductionRhsList* getRhsOfLhs(string lhsNonTerminalName);

private:
  GProductionList* _productionList;
  GEquationList* _equationList;
  GNonTerminal* _startSymbol;
};

class GTransformation {
public:
  /* eliminates symbol sym in all productions of grammar g
   * if the symbol is a nonterminal it is replaced by the rhs of the nonterminal */
  void eliminateNonTerminal(GGrammar* g,GNonTerminal* sym);
  /* the production must match exactly lhs->rhs; for rhs-lists each lhs->rhs_i is checked separately 
   * it deletes the production but also checks whether any production with the lhs is left. If not,
   * all occurences of the lhs are replaced by the rhs's of the rhs */
  void eliminateProduction(GGrammar* g, GProduction* p);

  /* deletes symbol sym in all productions of grammar g
   * if the symbol is a nonterminal on the rhs it is deleted and not replaced by the rhs of the symbol. 
   * if the symbol is a nonterminal on the lhs, the whole production is deleted
   * if the symnol is a terminal it is deleted, possibly leaving a rhs empty */
  void deleteSymbol(GGrammar* g,GSymbol* sym);
  /* deletes the production (lhs->rhs) */
  void deleteProduction(GGrammar* g, GProduction* p);
  /* adds production p to grammar g; the production can consist of rhs-list 
   * the function checks whether the lhs exists - if yes, it adds the rhs to this production
   * otherwise it adds a new production */
  void addProduction(GGrammar* g, GProduction* p);
  /* add a symbol newsym to a (tree grammar) symbol sym in grammar g; position 0=first, 1=second, ... 
   *  the function adds this new symbol to every occurence of symbol sym in the grammar */
  void addSymbolToSymbol(GGrammar* g, GSymbol* sym, GSymbol* newsym, unsigned int position);
  /* computes the set of empty productions and returns a list with all empty productions */
  GProductionList* findEmptyProductions(GGrammar* g);
  /* computes the number of productions with exactly one rhs */
  unsigned int productionsNum(GGrammar* g);
  /* computes the number of productions with different lhs */
  unsigned int bnfProductionsNum(GGrammar* g);
  /* tests whether two symbols have the same name (does not check arity, nor regex) */
  bool symbolsHaveSameName(GSymbol* s1, GSymbol* s2);
};

#endif
