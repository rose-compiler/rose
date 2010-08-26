#include "rose.h"
#include <VariableRenaming.h>

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	AstTests::runAllTests(project);

	VariableRenaming var_renaming(project);
	var_renaming.run();

	backend(project);
}
