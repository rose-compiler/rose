#include "rose.h"
//#include "FunctionInsert.h"

using namespace SageBuilder;
using namespace SageInterface;

class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:void visit(SgNode* astNode);
     bool done;
   };

void SimpleInstrumentation::visit(SgNode* astNode)
   {
  // SgFile *file=isSgFile(astNode);
     SgFunctionDefinition* funcdef = isSgFunctionDefinition(astNode);
     if (funcdef != NULL && !done)
        {
          printf ("In visit(): Found SgFunctionDefinition: funcdef->get_declaration()->get_name() = %s \n",funcdef->get_declaration()->get_name().str());

          SgScopeStatement *scope = getGlobalScope(funcdef);
#if 1
          printf ("Calling buildDefiningFunctionDeclaration() \n");
          SgFunctionDeclaration *func_defn = buildDefiningFunctionDeclaration(SgName("testFunc"),buildVoidType(),buildFunctionParameterList(buildInitializedName(SgName("param1"),buildIntType(),NULL)),scope);
#endif
#if 1
          reportNodesMarkedAsModified(func_defn);
#endif
#if 1
          printf ("Calling buildNondefiningFunctionDeclaration() \n");
          SgFunctionDeclaration *func_decl = buildNondefiningFunctionDeclaration(func_defn, scope);
#endif
#if 1
          reportNodesMarkedAsModified(func_decl);
#endif
          if (NULL != isSgGlobal(scope))
             {
               SgStatement *first_stmt = findFirstDefiningFunctionDecl(scope);

               printf ("Handling the global scope: first_stmt = %p \n",first_stmt);

               if (NULL == first_stmt)
                  {
                    first_stmt = getFirstStatement(scope);
                  }

               if (NULL != first_stmt)
                  {
                    printf ("Calling insertStatementBefore() \n");
#if 1
                    insertStatementBefore(first_stmt,func_decl);
#endif
                  }
                 else
                  {
                    printf ("Calling prependStatement() \n");
#if 1
                    prependStatement(func_decl,scope);
#endif
                  }
             }
            else
             {
               printf ("Handling the non-global scope \n");
#if 1
               prependStatement(func_decl, scope);
#endif
             }

#if 1
          reportNodesMarkedAsModified(scope);
#endif
#if 0
          SgProject::set_verbose(3);
#endif
          printf ("Calling insertStatementAfter() \n");
#if 0
          SgStatement *last_global_decl = findLastDeclarationStatement(scope);
          insertStatementAfter(last_global_decl, func_defn);
#else
          insertStatementAfter(funcdef->get_declaration(), func_defn);
#endif

#if 1
          reportNodesMarkedAsModified(scope);
#endif

          SgBasicBlock *func_body = func_defn->get_definition()->get_body();

#if 0
          SgProject::set_verbose(0);
#endif
          printf ("Calling buildVariableDeclaration() \n");
          SgVariableDeclaration *i = buildVariableDeclaration(SgName("i"),buildIntType(),buildAssignInitializer(buildIntVal(0),buildIntType()),func_body);

#if 1
          reportNodesMarkedAsModified(scope);
#endif

#if 0
          SgProject::set_verbose(3);
#endif
          printf ("Calling appendStatement() \n");
          appendStatement(i,func_body);

#if 1
          reportNodesMarkedAsModified(scope);
#endif
#if 0
          SgProject::set_verbose(0);
#endif
          done = true;

          printf ("Leaving visit(): SgFunctionDefinition: funcdef->get_declaration()->get_name() = %s \n",funcdef->get_declaration()->get_name().str());
        }
   }

int main(int argc, char *argv[])
   {
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project!=NULL);

  // SgProject::set_verbose(3);

     SimpleInstrumentation treeTraversal;
     treeTraversal.done = false;
     treeTraversal.traverseInputFiles(project, preorder);

  // SgProject::set_verbose(0);

  // DQ (4/15/2015): We should reset the isModified flags as part of the transformation 
  // because we have added statements explicitly marked as transformations.
  // checkIsModifiedFlag(project);

     return backend(project);
   }




