#ifndef FUNCTION_CALL_NORMALIZATION_H
#define FUNCTION_CALL_NORMALIZATION_H

#include <AstInterface.h>
#include <sstream>
#include <iostream>
#include <string>
// #include "rose.h"

struct Declaration {
  SgStatement *initVarDeclaration, *nonInitVarDeclaration, *assignment;
  SgName name;
};

typedef std::list<struct Declaration *> DeclarationPtrList;
typedef std::pair<SgFunctionCallExp*, SgExpression *> Fct2Var;

class FunctionCallNormalization : public SgSimpleProcessing
{
 public:
   // normalizes function calls of statements within a basic block
   void visit ( SgNode *astNode );

 private:
   void replaceFunctionCallsInExpression( SgNode *, std::map<SgFunctionCallExp *, SgExpression *> );

   // BFS query on an AST
   std::list<SgNode *> BFSQueryForNodes( SgNode *root, VariantT type );

   // function evaluation order query on an AST (first eval args, then function; for other nodes, it's postorder)
   std::list<SgNode *> FEOQueryForNodes( SgNode *root, VariantT type );

   std::list<SgNode *> createTraversalList( SgNode *root );
      
};

#endif
