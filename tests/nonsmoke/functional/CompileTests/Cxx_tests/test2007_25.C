/*
Using the following translator:

#include "rose.h"

int main( int argc, char * argv[] )
   {
     SgProject* project = frontend(argc,argv);
     VariantVector vv1(V_SgScopeStatement);
     std::list<SgNode*> memoryPoolTraversal = NodeQuery::queryMemoryPool(vv1);
     for(std::list<SgNode*>::iterator it_beg = memoryPoolTraversal.begin(); it_beg != memoryPoolTraversal.end(); ++it_beg)
        {
          SgScopeStatement*  scopeStmt = isSgScopeStatement(*it_beg);
          SgStatementPtrList scopeStmtPtrLst = scopeStmt->getStatementList();
        }
     return backend(project);
   }


I get the following error:

ERROR: default reached in switch (getStatementList())
Aborted

When running the same example using a bigger translator I get:

lt-macroRewrapper: Cxx_Grammar.C:37401: SgStatementPtrList&
SgScopeStatement::getStatementList(): Assertion currentScope != __null
failed.
*/



#define __STD_TYPE              __extension__ typedef

__STD_TYPE int __dev_t; /* Type of device numbers.  */

