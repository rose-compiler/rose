// This example shows how to construct a defining function (with a function body)
// using high level AST construction interfaces.
// A scope stack is used to pass scope information implicitly to some builder functions
#include "rose.h"
using namespace SageBuilder;
using namespace SageInterface;

int
main ( int argc, char * argv[] )
   {
 // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);
     SgGlobal *globalScope = getFirstGlobalScope (project);

 //push global scope into stack
     pushScopeStack (isSgScopeStatement (globalScope));

 // Create a parameter list with a parameter
    SgName var1_name = "var_name";
    SgReferenceType *ref_type = buildReferenceType(buildIntType());
    SgInitializedName *var1_init_name = buildInitializedName(var1_name, ref_type);
    SgFunctionParameterList* parameterList = buildFunctionParameterList();
    appendArg(parameterList,var1_init_name);

 // Create a defining functionDeclaration (with a function body)
    SgName func_name                    = "my_function";
    SgFunctionDeclaration * func        = buildDefiningFunctionDeclaration 
                  (func_name, buildIntType(), parameterList);
    SgBasicBlock*  func_body    = func->get_definition()->get_body();

 // push function body scope into stack
    pushScopeStack(isSgScopeStatement(func_body));

 // build a statement in the function body
    SgVarRefExp *var_ref = buildVarRefExp(var1_name);
    SgPlusPlusOp *pp_expression = buildPlusPlusOp(var_ref);
    SgExprStatement* new_stmt = buildExprStatement(pp_expression);
                     
 // insert a statement into the function body
    appendStatement(new_stmt);
//  pop function body off the stack
    popScopeStack();
    
//  insert the function declaration into the scope at the top of the scope stack    
    prependStatement(func);
    popScopeStack();

    AstTests::runAllTests(project);
    return backend(project);
 }
