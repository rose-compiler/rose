// DQ (9/16/2010): This test code demonstrate how to insert a statement
// before and after a function in a file.  Important to this test code 
// is that we correctly handle the and CPP directives that might be 
// attached to the first function declaration.

#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int
main (int argc, char *argv[])
   {
     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

     SgGlobal* globalScope = getFirstGlobalScope (project);

     Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);

     Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin();
     while (i != functionDeclarationList.end())
        {
          SgFunctionDeclaration* functionDelcaration = isSgFunctionDeclaration(*i);
          SgName functionName = functionDelcaration->get_name();

          if (functionName == "InsertStatementBeforeThisFunction")
             {
            // Build "InsertStatementBeforeThisFunction_var = variable_hidden_in_header_file;"
               SgVariableSymbol* variableSymbol = globalScope->lookup_variable_symbol("variable_hidden_in_header_file");
               SgExpression* varRefExp = buildVarRefExp(variableSymbol);
               SgAssignInitializer* initializer = new SgAssignInitializer(varRefExp,buildIntType());

               SgVariableDeclaration *varDecl = buildVariableDeclaration(functionName+"_var",buildIntType(),initializer);

            // Insert this variable declaration before this function
               bool insertBeforeNode = true;
               SageInterface::insertStatement(functionDelcaration, varDecl, insertBeforeNode);
             }

          if (functionName == "InsertStatementAfterThisFunction")
             {
            // Build "InsertStatementAfterThisFunction_var = InsertStatementAfterThisFunction();"
               SgFunctionSymbol* functionSymbol = globalScope->lookup_function_symbol(functionName);
               SgExpression* functionRefExp     = buildFunctionCallExp(functionSymbol);
               SgAssignInitializer* initializer = new SgAssignInitializer(functionRefExp,buildIntType());

               SgVariableDeclaration *varDecl = buildVariableDeclaration(functionName+"_var",buildIntType(),initializer);

            // Insert this variable declaration after this function
               bool insertBeforeNode = false;
               SageInterface::insertStatement(functionDelcaration, varDecl, insertBeforeNode);
             }

          i++;
        }

     return backend (project);
   }
