#include "restrictedLanguage/LanguageRestrictions.h"

#include "rose.h"
#include <vector>

using namespace std;

class visitorTraversal : public AstSimpleProcessing
{
public:
	vector<SgFunctionDefinition*> eventList;

	void visit(SgNode* n);

};


void visitorTraversal::visit(SgNode* node)
{
	SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(node);
	if (functionDeclaration != NULL)
	{
		// Detect restricted language events
		string functionName = functionDeclaration->get_name();
		if (functionName.find("EVENT_", 0) != string::npos)
		{
			SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(functionDeclaration->get_definition());
			if (functionDefinition != NULL)
			{
				eventList.push_back(functionDefinition);
			}
		}
	}
}


vector<SgFunctionDefinition*> buildEventList(SgProject* project)
{
	// This function collects the events in the input program.
	// Events for our simple language are always marked by "EVENT_"
	// as a prefix for the function names that correspond to and event.

	// Build the traversal object
	visitorTraversal restrictionTraversal;

	// Call the traversal starting at the project node of the AST
	restrictionTraversal.traverseInputFiles(project, preorder);

	return restrictionTraversal.eventList;
}


int main(int argc, char * argv[])
{
	SgProject* project = frontend(argc, argv);

	vector<SgFunctionDefinition*> eventList = buildEventList(project);

	return LanguageRestrictions::violatesRestrictionsOnEventFunctions(eventList);
}
