#include "staticSingleAssignment.h"
#include "rose.h"
#include "VariableRenaming.h"

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);

	if (project->get_frontendErrorCode() > 3)
	{
		//The frontend failed!
		return 1;
	}

	//Run the variable renaming on the project
	VariableRenaming varRenaming(project);
	varRenaming.run();

	if (SgProject::get_verbose() > 0)
	{
		printf("\n\n ***** VariableRenaming Complete ***** \n\n");
	}

	//Run the SSA analysis
	StaticSingleAssignment ssa(project);
	ssa.run();

	ROSE_ASSERT(ssa.getUseTable() == varRenaming.getUseTable());
	ROSE_ASSERT(ssa.getPropDefTable() == varRenaming.getPropDefTable());
	return 0;
}
