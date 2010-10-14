#ifndef CFGUTILS_H
#define CFGUTILS_H

#include "common.h"
#include "variables.h"
#include "VirtualCFGIterator.h"
#include "CFGRewrite.h"
#include "DataflowCFG.h"
//#include "DataflowCFG.cfgToDot.h"

namespace cfgUtils
{
  extern SgProject* project;

  // initializes the cfgUtils module
  void initCFGUtils(SgProject* project_arg);

  SgProject* getProject();

  // parses a logical condition, determines whether it is in the form (x <= y + c) and
  // if it is, sets x, y, c to be the unique ids of the relevant variables
  // If the condition is in a similar form (ex: x<y, x>c), x, y and c are normalized to the form x<=y+c
  // negX=true if x is supposed to be negated and false otherwise
  // negY=true if y is supposed to be negated and false otherwise
  bool computeTermsOfIfCondition_LTEQ(SgExpression *expr, varID &x, bool& negX, varID &y, bool& negY, long &c);

  // parses a logical condition, determines whether it is in the form (x == y + c) and
  // if it is, sets x, y, c to be the unique ids of the relevant variables
  // negX=true if x is supposed to be negated and false otherwise
  // negY=true if y is supposed to be negated and false otherwise
  bool computeTermsOfIfCondition_EQ(SgExpression *expr, varID &x, bool& negX, varID &y, bool& negY, long &c);

  // returns true if SgNode contains an array access to any variable in arrays
  // wrIndex - map of i+c pairs that containsfor each variable in arrays all the indexes 
  //    used to access the array variable for a Write operation
  // rdIndex - map of i+c pairs that containsfor each variable in arrays all the indexes 
  //    used to access the array variable for a Read operation
  // rdFromExp - map of flags indicating for each variable in arrays whether ast_node 
  //    contains an array Read expression of this array with its index in the wrong format: 
  //    something other than i or i+c
  // wrFromExp - map of flags indicating for each variable in arrays whether ast_node 
  //    contains an array Write expression with its index in the wrong format: something 
  //    other than i or i+c
  bool
    findArrayAccesses(SgNode* ast_node, 
        m_varID2varID2quad &wrIndex, m_varID2varID2quad &rdIndex,
        m_varID2bool &rdFromExp, m_varID2bool &wrFromExp, 
        varIDSet arrays, m_varID2str vars2Name);

  // examines the given expression. 
  // If it is an array access, fills wrIndex with the ids of variables on the left-hand-side 
  //    of the assignment, fills rdIndex with the ids of variables on the right-hand-side 
  //    of the assignment and returns true.
  // Otherwise, returns false.
  // wrIndexSimp, rdIndexSimp - maps of array variables to variable/constant pairs. Each pair 
  //    contains i,c that represent the fact that the given array variable was accessed (for 
  //    writing or reading, respectively using the index expression [i+c], where i is a variable 
  //    and c is a constant
  // wrIndexCpx, rdIndexCpx -  lists of array variables that were accessed (for writing or 
  //    reading, respectively) using a complex index expression
  bool parseArrayAccess(SgNode* ast_node,
      m_varID2varID2quad& wrIndexSimp, varIDlist& wrIndexCpx, 
      m_varID2varID2quad& rdIndexSimp, varIDlist& rdIndexCpx,
      m_varID2str vars2Name);

  // if the given SgNode is an assignment operation, returns true, otherwise false
  //// SgExpression that is that 
  //// assignment (effectively a type cast) or NULL otherwise
  /*SgExpression**/ bool isAssignment(SgNode* n);

  // if the given SgNode is an assignment operation, returns an SgNode that is the left-hand
  // side of this assignment and NULL otherwise
  SgNode* getAssignmentLHS(SgNode* n);

  // if the given SgNode is an assignment operation, adds to rhs the set of SgNodes that comprise the right-hand
  // side of this assignment
  void getAssignmentRHS(SgNode* n, set<SgNode*>& rhs);

  const short none=0;
  const short add=1;
  const short subtract=2;
  const short mult=3;
  const short divide=4;

  // Returns true if the expression is of the permitted type
  //    and sets i, j, k and c appropriately to represent an expression of the form
  //    i = j op k op c 
  //    where op may be either + (add), * (mult) or / (divide)
  // This function parses expressions such as i = j op k, i = j op c, i op= j, i++)
  bool parseAssignment(/*SgExpression*/SgNode* expr, short& op, varID &i, varID &j, bool& negJ, varID &k, bool& negK, long &c);

  // Returns true if the expression is of the permitted type
  //    and sets op, i, j and c appropriately to represent an expression of the form
  //    i op j op c 
  //    where op may be either + (add), * (mult) or / (divide)
  //    op may be = none if the rhs has only one term
  // This function parses non-assignment expressions such as i op j or c but not i = j op c, i op= j or i++
  bool parseExpr(SgExpression* expr, short& op, varID &i, bool &negI, varID& j, bool &negJ, long &c);

  // returns true if parsing was successful
  //    (expressions accepted: c, -c, j, c +/- j, j +/- c, j +/- k
  // and sets *j and *c appropriately
  // negJ=true if j is supposed to be negated and false otherwise
  // negK=true if k is supposed to be negated and false otherwise
  bool parseAddition(SgExpression* expr, varID &j, bool &negJ, varID& k, bool &negK, long &c);

  // returns true if parsing was successful
  //    (expressions accepted: c, -c, j, c * j, j * c, j * k)
  // and sets *j, *k and *c appropriately
  bool parseMultiplication(SgExpression* expr, varID &j, varID& k, long &c);

  // returns true if parsing was successful
  //    (expressions accepted: c, -c, j, c / j, j / c, j / k
  // and sets *j, *k and *c appropriately
  bool parseDivision(SgExpression* expr, varID &j, varID& k, long &c);

  /*// returns true if the given SgValueExp is some type of integral value, rather than a string or something more complex
    bool isIntegralVal(SgValueExp* exp);

  // if the given SgValueExp contains an integral value, returns that integer
  // no error return value, since caller supposed to check with isIntegralVal()
  int getIntegralVal(SgValueExp* exp);
  */
  // returns whether a given AST node that represents a constant is an integer and
  // sets *val to be the numeric value of that integer (all integer types are included
  // but not floating point, characters, etc.)
  bool IsConstInt (SgExpression* rhs, long &val);

  // pulls off all the SgCastExps that may be wrapping the given expression, returning the expression that is being wrapped
  SgExpression* unwrapCasts(SgExpression* e);

  // returns the DataflowNode that represents that start of the CFG of the given function's body
  DataflowNode getFuncStartCFG(SgFunctionDefinition* func);

  // returns the DataflowNode that represents that end of the CFG of the given function's body
  DataflowNode getFuncEndCFG(SgFunctionDefinition* func);

  // returns a string containing a unique name that is not otherwise used inside this project
  string genUniqueName();

  // returns the SgFunctionDeclaration for the function with the given name
  SgFunctionDeclaration* getFuncDecl(string name);

  // given a function's declaration, returns the function's definition.
  // handles the case where decl->get_definition()==NULL
  SgFunctionDefinition* funcDeclToDef(SgFunctionDeclaration* decl);

}

#endif
