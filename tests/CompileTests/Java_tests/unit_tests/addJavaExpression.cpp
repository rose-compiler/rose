// Expressions can be built using both bottomup (recommended ) and topdown orders.
// Bottomup: build operands first, operation later
// Topdown: build operation first, set operands later on.

#include <iostream>
#include "rose.h"
#include "ecj.h"
using namespace SageBuilder;
using namespace SageInterface;
using namespace std;


int main (int argc, char *argv[]) {
    SgProject *project = frontend (argc, argv);
    // go to the function body
    SgGlobal *global_scope = project -> get_globalScopeAcrossFiles();
    SgFunctionDeclaration* mainFunc = findMain(global_scope);

    SgBasicBlock* body= mainFunc -> get_definition() -> get_body();
    pushScopeStack(body);
  
    SgExpression *init_exp =  
            buildMultiplyOp(buildDoubleVal(2.0),
                 buildSubtractOp(buildDoubleVal(1.0), 
                      buildMultiplyOp (buildVarRefExp("gamma"),buildVarRefExp("gamma")
                                      )));
    SgVariableDeclaration *decl = buildVariableDeclaration("result",buildDoubleType(),buildAssignInitializer(init_exp));
    decl -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    SgStatement* laststmt = getLastStatement(topScopeStack());
    insertStatementBefore(laststmt,decl);

    SgExpression * init_exp2 = buildMultiplyOp();
    setLhsOperand(init_exp2,buildVarRefExp("alpha"));
    setRhsOperand(init_exp2,buildVarRefExp("beta"));

    SgVariableDeclaration *decl2 = buildVariableDeclaration("result2",buildDoubleType(),buildAssignInitializer(init_exp2));

    laststmt = getLastStatement(topScopeStack());
    insertStatementBefore(laststmt,decl2);
    decl2 -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    SgExpression *init_exp3 = SageBuilder::buildStringVal("");

  //
  // Rose always makes 3 Java classes available: java.lang.Object, java.lang.String, java.lang.Class.
  // If the user wants to use another class, he must first import it into the project.
  // See java.C example for how to use the processImport(...) function.
  //
    SgClassDefinition *package_definition = findOrInsertJavaPackage(project, (string) "java.lang");
    ROSE_ASSERT(package_definition);
    SgClassSymbol *class_symbol = package_definition -> lookup_class_symbol("String");
    SgClassType *string_type = isSgClassType(class_symbol -> get_type());

    SgVariableDeclaration *decl3 = buildVariableDeclaration("result3", string_type, buildAssignInitializer(init_exp3));

    laststmt = getLastStatement(topScopeStack());
    insertStatementBefore(laststmt,decl3);
    decl3 -> get_declarationModifier().get_accessModifier().set_modifier(SgAccessModifier::e_unknown);

    popScopeStack();

    AstTests::runAllTests(project);

  //invoke backend compiler to generate object/binary files
    return backend (project);
}

