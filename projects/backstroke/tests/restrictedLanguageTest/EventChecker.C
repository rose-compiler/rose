#include "restrictedLanguage/LanguageRestrictions.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

#include "rose.h"
#include <vector>

using namespace std;


int main(int argc, char * argv[])
{
	SgProject* project = frontend(argc, argv);

	vector<SgFunctionDefinition*> eventList = SageInterface::querySubTree<SgFunctionDefinition>(project, V_SgFunctionDefinition);

	foreach(SgFunctionDefinition* function, eventList)
	{
		LanguageRestrictions::violatesRestrictionsOnEventFunctions(function);
	}

	return 0;
}
