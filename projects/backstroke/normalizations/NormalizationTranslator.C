#include "rose.h"
#include "ExtractFunctionArguments.h"
#include "FunctionNormalization.h"

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	AstTests::runAllTests(project);

	//Generate a DOT file so we see what's going on
	generateDOT(*project);
	generatePDF(*project);
	generateAstGraph(project, 2000);
	
	//Normalize here
	ExtractFunctionArguments e;
	e.NormalizeTree(project);

	//Try out some crazy C++ constructs
	//for (for(;;){}, ; ;); //For loop inside of for loop initializer: not allowed
	//double y = (double x = abs(12)); //Declaration inside declaration: not allowed
	//double y = (3 * 2, abs(4), 5); //Sequencing operator with function calls: works
	// (3 * 2, int x = 3); //Declaration inside sequencing operator: not allowed
	/*for (int i = 2, j = 0; int k = 23; i++)
	{
	 This loop is valid code.
	}*/

	AstTests::runAllTests(project);
	return backend(project);
}
