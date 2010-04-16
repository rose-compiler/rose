// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;



class visitorTraversal : public AstSimpleProcessing
{
public:
    virtual void visit(SgNode* n);

private:
	SgFunctionRefExp* trackVarRefToFindFunction(SgVarRefExp* ref);
};

void visitorTraversal::visit(SgNode* n)
{
	SgExpression* exp = NULL;
	SgFunctionDeclaration* func_decl = NULL;

	// there are two ways to assign the event handler to the event member of lp object
	// first check if it is assigned by initializer
	SgAssignInitializer* ai = isSgAssignInitializer(n);
	if(ai && get_name(ai->get_type()) == "event_f")
		exp = ai->get_operand();
	else {
		// then check if it is through assign operator
		SgAssignOp* ao = isSgAssignOp(n);
		if(ao && get_name(ao->get_type()) == "event_f")  
			exp = ao->get_rhs_operand();
	}


	SgFunctionRefExp* fexp = NULL;
	//SgVarRefExp* vrexp = NULL;

	// if there is a cast, we get the operand of it
	SgCastExp* cexp = isSgCastExp(exp);
	if(cexp) 
		//exp = cexp->get_operand();

	//// then exp is either the function itself or another function pointer variable
	//SgVarRefExp* vrexp = isSgVarRefExp(exp);
	//if(vrexp)

		fexp = isSgFunctionRefExp(cexp->get_operand());
	else
		fexp = isSgFunctionRefExp(exp);
	
	if(fexp) {
		func_decl = fexp->getAssociatedFunctionDeclaration();
		cout << get_name(func_decl) << endl;
	}
}

/* 
visitorTraversal::SgFunctionRefExp* trackVarRefToFindFunction(SgVarRefExp* ref)
{
	get_symbol()->get_declaration()->get_definition()
}
*/

// Typical main function for ROSE translator
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

