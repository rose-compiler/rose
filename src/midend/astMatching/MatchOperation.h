#ifndef MATCHOPERATION_H
#define MATCHOPERATION_H

/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <string>
#include <list>
#include <sstream>
#include <iostream>
#include <set>
#include <map>
#include "RoseAst.h"

typedef std::map<std::string,SgNode*> SingleMatchVarBindings;
typedef RoseAst::iterator SingleMatchMarkedLocation;
typedef std::list<SingleMatchMarkedLocation> SingleMatchMarkedLocations;
typedef std::list<SingleMatchVarBindings> MatchResult;

class MatchOpSequence;

struct SingleMatchResult {
  SingleMatchVarBindings singleMatchVarBindings;
  SingleMatchMarkedLocations singleMatchMarkedLocations;
  bool success;
  void clear();
  bool isSMRMarkedLocation(RoseAst::iterator& i);
};

class MatchStatus {
 public:
  MatchStatus():debug(false),_allMatchVarBindings(0){
    resetAllMatchVarBindings();
    resetAllMarkedLocations();
  }
  ~MatchStatus() {
    delete _allMatchVarBindings; 
  }
  enum PatternMatchMode {MATCHMODE_SHALLOW, MATCHMODE_DEEP, MATCHMODE_SINGLE};
  enum CheckNodeMode {NODECHECKMODE_TYPEID,NODECHECKMODE_VARIANT};
  bool isMarkedLocationAddress(RoseAst::iterator& i);
  void resetAllMatchVarBindings();
  void resetAllMarkedLocations();
 public:
  bool debug;
  void mergeOtherStatus(MatchStatus& other);
  void mergeSingleMatchResult(SingleMatchResult& other);
  std::list<SingleMatchVarBindings>* _allMatchVarBindings;
  SingleMatchMarkedLocations _allMatchMarkedLocations;

  /* adds a single var binding to map of var bindings */
  void addVarBinding(std::string varname,SgNode* node);
  /* adds a single marked location to set of marked locations */
  void addMarkedLocation(SingleMatchMarkedLocation locIter);
  
  /* updates state to include new match result and resets all data
     to be ready for new match */
  void commitSingleMatchResult();
 private:
  SingleMatchResult current_smr;
  bool _keepMarkedLocations;
};  

class MatchOperation {
 public:
  virtual std::string toString()=0;
  virtual bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb);
};

class MatchOpSequence : public std::list<MatchOperation*>{
  // we are using default std::list constructors
 public:
  std::string toString();
  bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb);
};

class MatchOpOr : public MatchOperation {
 public:
 MatchOpOr(MatchOpSequence* l, MatchOpSequence* r):_left(l),_right(r){}
  std::string toString();
  bool performOperation(MatchStatus& status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
  MatchOpSequence* _left;
  MatchOpSequence* _right;
};

class MatchOpVariableAssignment : public MatchOperation {
 public:
  MatchOpVariableAssignment(std::string varName);
  std::string toString();
  bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
  std::string _varName;
};

class MatchOpCheckNode : public MatchOperation {
 public:
  MatchOpCheckNode(std::string nodename);
  std::string toString();
  bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
  std::string _nodename;
};

class MatchOpCheckNodeSet : public MatchOperation {
 public:
  MatchOpCheckNodeSet(std::string nodenameset);
  std::string toString();
  bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
  std::string _nodenameset;
};

class MatchOpArityCheck : public MatchOperation {
 public:
  MatchOpArityCheck(size_t arity);
  MatchOpArityCheck(size_t minarity, size_t maxarity);
  std::string toString();
  bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
  size_t _minarity;
  size_t _maxarity;
};

/* 
for (size_t idx = 0; idx < numberOfSuccessors; idx++)
child = node->get_traversalSuccessorByIndex(idx);
node->get_numberOfTraversalSuccessors();
*/

/* This operation skips the subtree of the current node. This is equivalent
to assuming that this node has no children. It also performs one forward operation.
*/

class MatchOpForward : public MatchOperation {
 public:
  MatchOpForward();
  std::string toString();
  bool performOperation(MatchStatus& status, RoseAst::iterator& i, SingleMatchResult& vb);
};

class MatchOpSkipChildOnForward : public MatchOperation {
 public:
  MatchOpSkipChildOnForward();
  std::string toString();
  bool performOperation(MatchStatus& status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
};

class MatchOpMarkNode : public MatchOperation {
 public:
  MatchOpMarkNode();
  std::string toString();
  bool performOperation(MatchStatus& status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
};

class MatchOpCheckNull : public MatchOperation {
 public:
  MatchOpCheckNull();
  std::string toString();
  bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
};

class MatchOpDotDot : public MatchOperation {
 public:
  MatchOpDotDot();
  std::string toString();
  bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb);
 private:
};
typedef MatchOpSequence MatchOperationList; // TODO: eliminate type alias
typedef std::list<MatchOperationList*> MatchOperationListList;

//#include "matcherparser.h" // we are using yytokentype in MatchOpBinaryOp
// TODO: fake type for testing

class MatchOpBinaryOp : public MatchOperation {
 public:
 MatchOpBinaryOp(int op,MatchOperation* l,MatchOperation* r):_op(op),_left(l),_right(r) {}
  std::string toString() { return "binop()";}
  bool performOperation(MatchStatus&  status, RoseAst::iterator& i, SingleMatchResult& vb) {
    switch(_op) {
#if 0
    case C_NEQ: return _left->performOperation(status,i,vb) != _right->performOperation(status,i,vb);
    case C_EQ:  return _left->performOperation(status,i,vb) == _right->performOperation(status,i,vb);
#endif
    default: throw "Error: unknown operation in where-expression.";
    }
  }
  private:
  int _op;
  MatchOperation* _left;
  MatchOperation* _right;
};

class MatchOpUnaryOp : public MatchOperation {
 public:
  MatchOpUnaryOp(int,MatchOperation* o) {}
  std::string toString() { return "not-implemented-yet";}  
};
class MatchOpConstant : public MatchOperation {
 public:
  MatchOpConstant(int) {}
  std::string toString() { return "not-implemented-yet";}  
};


class MatchOpAccessVariable : public MatchOperation {
 public:
  MatchOpAccessVariable(std::string) {}
  std::string toString() { return "not-implemented-yet";}  
};
class MatchOpAccessRoseAstAttribute : public MatchOperation {
 public:
  MatchOpAccessRoseAstAttribute(std::string, std::string) {}
  std::string toString() { return "not-implemented-yet";}  
};
class MatchOpAccessUserAstAttribute : public MatchOperation {
 public:
  MatchOpAccessUserAstAttribute(std::string, std::string) {}
  std::string toString() { return "not-implemented-yet";}  
};

#endif
