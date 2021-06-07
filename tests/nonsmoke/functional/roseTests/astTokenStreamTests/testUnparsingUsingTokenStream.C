#include "rose.h"
//#include "FunctionInsert.h"

using namespace SageBuilder;
using namespace SageInterface;

class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:void visit(SgNode* astNode);
     bool done;
   };


#if 0
// DQ (5/20/2021): This is part of a debugging test.
extern int* target_physicial_file_id_ptr;
extern SgFunctionDeclaration* target_function_declaration_ptr;
#endif

void SimpleInstrumentation::visit(SgNode* astNode)
   {
  // SgFile *file=isSgFile(astNode);
     SgFunctionDefinition* funcdef = isSgFunctionDefinition(astNode);
     if (funcdef != NULL && !done)
        {
          printf ("In visit(): Found SgFunctionDefinition: funcdef->get_declaration()->get_name() = %s \n",funcdef->get_declaration()->get_name().str());

          SgScopeStatement *scope = getGlobalScope(funcdef);
#if 0
          printf ("Calling buildDefiningFunctionDeclaration() \n");
#endif
       // DQ (11/9/2015): Unless this is marked to be output in code generaton, this function will not show up in the output.
          SgFunctionDeclaration *func_defn = buildDefiningFunctionDeclaration(SgName("testFunc"),buildVoidType(),buildFunctionParameterList(buildInitializedName(SgName("param1"),buildIntType(),NULL)),scope);

#if 0
       // DQ (5/20/2021): Testing the frontier computation.
          target_function_declaration_ptr = func_defn;
#endif
#if 0
          reportNodesMarkedAsModified(func_defn);
#endif
#if 0
          printf ("Calling buildNondefiningFunctionDeclaration() \n");
#endif
          SgFunctionDeclaration *func_decl = buildNondefiningFunctionDeclaration(func_defn, scope);
#if 0
          reportNodesMarkedAsModified(func_decl);
#endif
       // if (NULL != isSgGlobal(scope))
          if (isSgGlobal(scope) != NULL)
             {
               SgStatement *first_stmt = findFirstDefiningFunctionDecl(scope);
#if 0
               printf ("Handling the global scope: first_stmt = %p \n",first_stmt);
#endif
               if (NULL == first_stmt)
                  {
                    first_stmt = getFirstStatement(scope);
                  }

               if (NULL != first_stmt)
                  {
#if 0
                    printf ("Calling insertStatementBefore() \n");
#endif
                 // DQ (11/9/2015): Unless this is marked to be output in code generaton, this function will not show up in the output.
                    insertStatementBefore(first_stmt,func_decl);
                  }
                 else
                  {
#if 0
                    printf ("Calling prependStatement() \n");
#endif
                 // DQ (11/9/2015): Unless this is marked to be output in code generaton, this function will not show up in the output.
                    prependStatement(func_decl,scope);
                  }
             }
            else
             {
#if 0
               printf ("Handling the non-global scope \n");
#endif
            // DQ (11/9/2015): Unless this is marked to be output in code generaton, this function will not show up in the output.
               prependStatement(func_decl, scope);
             }

#if 0
          reportNodesMarkedAsModified(scope);
#endif
#if 0
          SgProject::set_verbose(3);
#endif
#if 0
          printf ("Calling insertStatementAfter() \n");
#endif
#if 0
          SgStatement *last_global_decl = findLastDeclarationStatement(scope);
          insertStatementAfter(last_global_decl, func_defn);
#else
       // DQ (11/9/2015): Unless this is marked to be output in code generaton, this function will not show up in the output.
          insertStatementAfter(funcdef->get_declaration(), func_defn);
#endif
#if 0
          reportNodesMarkedAsModified(scope);
#endif
          SgBasicBlock *func_body = func_defn->get_definition()->get_body();
#if 0
          SgProject::set_verbose(0);
#endif
#if 0
          printf ("Calling buildVariableDeclaration() \n");
#endif
          SgVariableDeclaration *i = buildVariableDeclaration(SgName("i"),buildIntType(),buildAssignInitializer(buildIntVal(0),buildIntType()),func_body);

#if 0
          reportNodesMarkedAsModified(scope);
#endif

#if 0
          SgProject::set_verbose(3);
#endif
       // DQ (11/9/2015): Unless this is marked to be output in code generaton, this function will not show up in the output.
#if 0
          printf ("Calling appendStatement() \n");
#endif
          appendStatement(i,func_body);
#if 0
          int before_physical_file_id = func_defn->get_file_info()->get_physical_file_id();
          printf ("before_physical_file_id = %d \n",before_physical_file_id);
#endif
       // DQ (5/19/2021): Mark the subtree as being from same file as the scope.
          int physical_file_id = scope->get_file_info()->get_physical_file_id();
#if 0
          printf ("Reset func_defn physical_file_id using physical_file_id = %d \n",physical_file_id);
#endif
       // DQ (5/20/2021): We need to set the physical file id for boththe defining and firstnondefining declarations.
          SageInterface::markSubtreeToBeUnparsed(func_defn,physical_file_id);
          SageInterface::markSubtreeToBeUnparsed(func_decl,physical_file_id);
#if 0
          int after_physical_file_id = func_defn->get_file_info()->get_physical_file_id();
          printf ("after_physical_file_id = %d \n",after_physical_file_id);

       // DQ (5/20/2021): Testing the frontier computation.
          target_physicial_file_id_ptr = func_defn->get_file_info()->get_physical_file_id_reference();

       // DQ (5/20/2021): Testing the frontier computation.
          ROSE_ASSERT(target_function_declaration_ptr != NULL);
          printf ("target_function_declaration_ptr = %p = %s name = %s \n",target_function_declaration_ptr,
               target_function_declaration_ptr->class_name().c_str(),target_function_declaration_ptr->get_name().str());
          int target_function_declaration_physical_file_id = target_function_declaration_ptr->get_file_info()->get_physical_file_id();
          printf ("target_function_declaration_physical_file_id = %d \n",target_function_declaration_physical_file_id);
#endif
#if 0
          reportNodesMarkedAsModified(scope);
#endif
#if 0
          SgProject::set_verbose(0);
#endif
          done = true;
#if 0
          printf ("Leaving visit(): SgFunctionDefinition: funcdef->get_declaration()->get_name() = %s \n",funcdef->get_declaration()->get_name().str());
#endif
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

#if 1
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
  // DQ (12/22/2019): Call multi-file version (instead of generateDOT() function).
  // generateAstGraph(project, 2000);
  // generateDOT ( *project );
     generateDOTforMultipleFile(*project);
#endif

     return backend(project);
   }




