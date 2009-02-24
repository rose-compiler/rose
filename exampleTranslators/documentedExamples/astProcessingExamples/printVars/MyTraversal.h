// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: MyTraversal.h,v 1.3 2008/01/08 02:55:50 dquinlan Exp $

#ifndef PRINTVARS_H
#define PRINTVARS_H



// using namespace std;

/* The class MyTraversal inherits from AstSimpleProcessing and
   overrides the method 'visit' to perform an action at each AST
   node. In method 'visit' the names of all variables that are used in
   expressions are printed to stdout and the data member variable
   'binaryOpCount' is used for counting all traversed binary operator
   nodes (accumulator).
*/
class MyTraversal : public AstSimpleProcessing {
public:
  MyTraversal():binaryOpCount(0) {}
  unsigned int getBinaryOpCount();
protected:
  void visit(SgNode*);
private:
  //! used as accumulator
  unsigned int binaryOpCount;
};

#endif
