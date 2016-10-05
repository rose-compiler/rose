// ROSE Translator to make sure that Fortran parameters come through as const
#include "rose.h"
#include <boost/foreach.hpp>
using namespace SageBuilder;
using namespace SageInterface;

int main(int argc, char* argv[])
{
	SgProject* project = frontend(argc, argv);

	std::vector<SgInitializedName*> names = SageInterface::querySubTree<SgInitializedName>(project, V_SgInitializedName);
	BOOST_FOREACH(SgInitializedName* name, names)
	{
		if (name->get_qualified_name().getString() == "pi")
		{
			if (!SageInterface::isConstType(name->get_type()))
			{
				std::cerr << "Error, parameter 'pi' is not const." << std::endl;
				return 1;
			}
		}
	}

	return 0;
}

