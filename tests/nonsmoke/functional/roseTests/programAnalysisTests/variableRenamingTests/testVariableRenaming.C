#include "rose.h"
#include <VariableRenaming.h>

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);

	//If there is an error in the frontend, just skip the test.
	if (project->get_frontendErrorCode() > 3)
		return 0;

	AstTests::runAllTests(project);

	VariableRenaming var_renaming(project);
	var_renaming.run();
}
