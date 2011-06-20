#include "rose.h"
#include "CallGraph.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

using namespace std;

int main(int argc, char * argv[])
{
	SgProject* project = new SgProject(argc, argv);

	//Construct class hierarchy graph
	ClassHierarchyWrapper hier(project);

	//Display the ancestors of each class
	vector<SgClassDefinition*> allClasses = SageInterface::querySubTree<SgClassDefinition>(project, V_SgClassDefinition);
	foreach(SgClassDefinition* classDef, allClasses)
	{
		printf("\n%s subclasses: ", classDef->get_declaration()->get_name().str());
		foreach(SgClassDefinition* subclass, hier.getSubclasses(classDef))
		{
			printf("%s, ", subclass->get_declaration()->get_name().str());
		}
	}

	return 0;
}

