// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

class visitorTraversal : public AstSimpleProcessing
{
public:
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* n)
{
    SgExpression* exp = NULL;

    // there are two ways to assign the event handler to the event member of lp object
    // first check if it is assigned by initializer
    if (SgAssignInitializer* init = isSgAssignInitializer(n))
    {
	if(get_name(init->get_type()) == "event_f")
	    exp = init->get_operand();
    }
    // then check if it is through assign operator
    else if (SgAssignOp* asn_op = isSgAssignOp(n))
    {
	if(get_name(asn_op->get_type()) == "event_f")  
	    exp = asn_op->get_rhs_operand();
    }

    if (exp == NULL) return;

    SgFunctionRefExp* func_exp = NULL;

    // if there is a cast, we get the operand of it
    if(SgCastExp* cexp = isSgCastExp(exp)) 
	func_exp = isSgFunctionRefExp(cexp->get_operand());
    else
	func_exp = isSgFunctionRefExp(exp);

    SgFunctionDeclaration* func_decl = NULL;
    if(func_exp) 
    {
	func_decl = func_exp->getAssociatedFunctionDeclaration();
	cout << get_name(func_decl) << endl;
    }
}

int main( int argc, char * argv[] )
{
    // Build the AST used by ROSE
    SgProject* project = frontend(argc,argv);

    // Build the traversal object
    visitorTraversal exampleTraversal;

    // Call the traversal starting at the project node of the AST
    exampleTraversal.traverseInputFiles(project,preorder);

    return backend(project);
}

