// ROSE Translator to make sure that Fortran parameters come through as const
#include "rose.h"
#include <boost/foreach.hpp>
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH

int main(int argc, char* argv[])
{
	SgProject* project = frontend(argc, argv);

	std::vector<SgInitializedName*> names = SageInterface::querySubTree<SgInitializedName>(project, V_SgInitializedName);
	std::vector<SgInitializedName*> pnames;

	foreach(SgInitializedName* name, names)
	{
		std::cout << name->get_qualified_name().getString();
		if (name->get_protected_declaration())
		{
			std::cout << " is ";
			pnames.push_back(name);
		}
		else
			std::cout << " is not ";
		std::cout << "protected." << std::endl;
#if 0
		if (name->get_qualified_name().getString() == "val")
		{
			if (!SageInterface::isConstType(name->get_type()))
			{
				std::cerr << "Error, parameter 'val' is not const." << std::endl;
				return 1;
			}
		}
		else if (name->get_qualified_name().getString() == "age")
		{
			if (!SageInterface::isConstType(name->get_type()))
			{
				std::cerr << "Error, parameter 'age' is not const." << std::endl;
				return 1;
			}
		}
#endif
	}

	std::vector<SgAssignOp*> assigns = SageInterface::querySubTree<SgAssignOp>(project);
	foreach(SgAssignOp* op, assigns)
	{
		SgVarRefExp* var = isSgVarRefExp(op->get_lhs_operand());
		SgInitializedName* decl = var->get_symbol()->get_declaration();
		std::cout << "Found assignment to " << var->get_symbol()->get_name().str();
		if (decl->get_protected_declaration())
		{
			std::cout << ", which is protected.";
		}
		std::cout << "\t\t" << op->unparseToString();
		std::cout << std::endl;
		SgNode* assign_scope = SageInterface::getScope(op);
		SgNode* var_scope = decl->get_scope();
		if (SageInterface::isAncestor(var_scope, assign_scope))
		{
			std::cout << "\tAnd both are in the same scope.\n";
		}
		else
		{
			std::cout << "\tIn different scopes.\n";
		}
	}

	return 0;
}

