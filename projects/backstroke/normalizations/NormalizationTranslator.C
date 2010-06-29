#include "rose.h"
#include "ExtractFunctionArguments.h"
#include "FunctionNormalization.h"
#include "CFGReverserProofOfConcept.h"

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	AstTests::runAllTests(project);

	//Generate a DOT file so we see what's going on
	generateDOT(*project);
	generatePDF(*project);
	generateAstGraph(project, 2000);

	/*CFGReverserProofofConcept reverser(project);
	SgFunctionDeclaration* mainDeclaration = isSgFunctionDeclaration(SageInterface::findMain(project)->get_definingDeclaration());
	reverser.ReverseFunctionBody(mainDeclaration->get_definition()->get_body());*/

	//Normalize here
	ExtractFunctionArguments e;
	e.NormalizeTree(project);

	AstTests::runAllTests(project);
	return backend(project);
}
