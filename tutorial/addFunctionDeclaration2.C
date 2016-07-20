// This example shows how to construct a defining function (with a function body)
// using high level AST construction interfaces.
//
#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:
          void visit ( SgNode* astNode );
   };

void
SimpleInstrumentation::visit ( SgNode* astNode )
   {
     SgGlobal* globalScope = isSgGlobal(astNode);
     if (globalScope != NULL)
        {
       // ********************************************************************
       // Create a parameter list with a parameter
       // ********************************************************************
          SgName var1_name = "var_name";
          SgReferenceType *ref_type = buildReferenceType(buildIntType());
          SgInitializedName *var1_init_name = buildInitializedName(var1_name, ref_type);
          SgFunctionParameterList* parameterList = buildFunctionParameterList();
          appendArg(parameterList,var1_init_name);

       // *****************************************************
       // Create a defining functionDeclaration (with a function body)
       // *****************************************************
          SgName func_name                    = "my_function";
          SgFunctionDeclaration * func        = buildDefiningFunctionDeclaration 
                        (func_name, buildIntType(), parameterList,globalScope);
          SgBasicBlock*  func_body    = func->get_definition()->get_body();

       // ********************************************************
       // Insert a statement in the function body
       // *******************************************************

          SgVarRefExp *var_ref = buildVarRefExp(var1_name,func_body);
          SgPlusPlusOp *pp_expression = buildPlusPlusOp(var_ref);
          SgExprStatement* new_stmt = buildExprStatement(pp_expression);
			   
       // insert a statement into the function body
          prependStatement(new_stmt,func_body);
          prependStatement(func,globalScope);
       
        }
   }

int
main ( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

     SimpleInstrumentation treeTraversal;
     treeTraversal.traverseInputFiles ( project, preorder );

     AstTests::runAllTests(project);
     return backend(project);
   }
