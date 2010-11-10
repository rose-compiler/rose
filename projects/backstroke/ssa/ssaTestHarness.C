#include "staticSingleAssignment.h"
#include "rose.h"
#include "VariableRenaming.h"

class ComparisonTraversal : public AstSimpleProcessing
{
public:
	
	StaticSingleAssignment* ssa;
	VariableRenaming* varRenaming;
	
	virtual void visit(SgNode* node)
	{
		ROSE_ASSERT(ssa->getOriginalDefsAtNode(node) == varRenaming->getOriginalDefsAtNode(node));
		ROSE_ASSERT(ssa->getExpandedDefsAtNode(node) == varRenaming->getExpandedDefsAtNode(node));
	}
};

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

	ComparisonTraversal t;
	t.varRenaming = &varRenaming;
	t.ssa = &ssa;
	t.traverse(project, preorder);

	return 0;
}
