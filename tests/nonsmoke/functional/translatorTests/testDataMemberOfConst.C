// ROSE Translator to make sure that members of const data structures are const
#include "rose.h"
#include <boost/foreach.hpp>
using namespace SageBuilder;
using namespace SageInterface;

int main(int argc, char* argv[])
{
	SgProject* project = frontend(argc, argv);

	SgFunctionDeclaration* mainFunc= findMain(project);
	SgBasicBlock* body= mainFunc->get_definition()->get_body();

	std::vector<SgDotExp*> dots = SageInterface::querySubTree<SgDotExp>(body, V_SgDotExp);
	BOOST_FOREACH(SgDotExp* dot, dots)
	{
		std::string memberName = isSgVarRefExp(dot->get_rhs_operand())->get_symbol()->get_name().getString();
		if (memberName == "i")
		{
			if (!SageInterface::isConstType(dot->get_type()))
			{
				std::cerr << "Error, member 'i' is not const." << std::endl;
				return 1;
			}
		}
		else if (memberName == "mi")
		{
			if (SageInterface::isConstType(dot->get_type()))
			{
				std::cerr << "Error, member 'mi' is const." << std::endl;
				return 2;
			}
		}
	}

	return 0;
}


