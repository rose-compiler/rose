// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"

struct CheckUniqueNames: AstSimpleProcessing {
    void visit(SgNode *node) {

        printf ("In CheckUniqueNames::visit(): node = %p = %s \n",node,node->class_name().c_str());

        if (SgReturnStmt *stmt = isSgReturnStmt(node)) {
            SgExpression *expr = stmt->get_expression();
            assert(expr!=NULL);
            AstAttribute *attr = expr->getAttribute("UniqueNameAttribute");
            assert(attr!=NULL);
            SageInterface::UniqueNameAttribute* uAttr = dynamic_cast<SageInterface::UniqueNameAttribute*>(attr);
            assert(uAttr!=NULL);
            SgFunctionDefinition *fdef = SageInterface::getEnclosingNode<SgFunctionDefinition>(expr);
            assert(fdef!=NULL);
            std::cout <<"----------------------------------------\n"
                      <<"statement = (" <<stmt->class_name() <<"*)" <<stmt <<"\n"
                      <<"return expression = (" <<expr->class_name() <<"*)" <<expr <<"\n"
                      <<"return expression name = " <<uAttr->get_name() <<"\n"
                      <<"statement appears in a " <<fdef->class_name() <<"\n";
        }
    }
};

int main( int argc, char * argv[] ) 
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);

  // Run internal consistency tests on AST
     AstTests::runAllTests(project);

  // Insert your own manipulation of the AST here...
     SageInterface::annotateExpressionsWithUniqueNames(project);
     CheckUniqueNames().traverse(project, preorder);
     generateAstGraph(project, 10000);
     //generateDOT(*project);

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }

