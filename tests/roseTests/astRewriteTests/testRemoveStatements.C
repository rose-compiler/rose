// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

class MyTraversal
   : public SgSimpleProcessing
   {
     public:
          void visit(SgNode* astNode);
   };

void 
MyTraversal::visit ( SgNode* astNode )
   {
  // Some nodes are part of compond statements (SgForStatement contains a 
  // SgForInitStatement and can't be unparsed without it)
  // So there are some restrictions (I think) as to what sorts of
  // nodes can be removed.

  // Nodes to currently avoid (restrictions on the remove mechanism)
  // since the unparse mechanism needs to see these in order to work
#if 1
//   bool skipRemove = (isSgGlobal(astNode)                         != NULL);
     bool skipRemove = false;
#else
     bool skipRemove = (isSgGlobal(astNode)                         != NULL) ||
                       (isSgForStatement(astNode->get_parent())     != NULL) ||
                       (isSgForInitStatement(astNode->get_parent()) != NULL) ||
                       (isSgFunctionParameterList(astNode)          != NULL) ||
                       (isSgBasicBlock(astNode)                     != NULL) ||
                       (isSgFunctionDefinition(astNode)             != NULL) ||
                       (isSgFunctionDeclaration(astNode)            != NULL);
#endif

     SgStatement* statement = isSgStatement(astNode);
  // if ( (statement != NULL) && (!skipRemove) )
     if ( (statement != NULL) && (!skipRemove) )
        {
          printf ("Removing statement = %s \n",statement->sage_class_name());
//        string statementSourceCode = statement->unparseToString();
//        printf ("Removing statementSourceCode = %s \n",statementSourceCode.c_str());
#if 0
          HighLevelRewrite::remove(statement);
#endif
#if 0
          MiddleLevelRewrite::remove(statement);
#endif
#if 1
          LowLevelRewrite::remove(statement);
#endif
#if 0
          SgStatement * parentst = isSgStatement(statement->get_parent());
          if(parentst)
             {
               parentst->remove_statement(statement);
             }
#endif
        }
   }

int
main( int argc, char * argv[] )
   {
     SgProject* project= frontend (argc,argv); 

     MyTraversal myTraversal;
     myTraversal.traverseInputFiles (project,postorder);

     return backend(project);
   }

