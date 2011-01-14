#include <backstroke.h>
#include <VariableRenaming.h>
#include "utilities/cppDefinesAndNamespaces.h"
#include "normalizations/expNormalization.h"
#include "pluggableReverser/eventProcessor.h"
#include "pluggableReverser/expAndStmtHandlers.h"

using namespace std;

struct IsEvent
{
	SgScopeStatement* globalScope;

	IsEvent(SgScopeStatement* scope)
	: globalScope(scope) {}

	bool operator() (SgFunctionDeclaration* decl)
	{
		if (SgMemberFunctionDeclaration* memFunc = isSgMemberFunctionDeclaration(decl))
		{
			SgClassDefinition* classDef = memFunc->get_class_scope();
			SgClassDeclaration* classDecl = classDef->get_declaration();
			string className = classDecl->get_name();

			SgNamespaceDefinitionStatement* namespaceDef = isSgNamespaceDefinitionStatement(classDecl->get_parent());
			if (namespaceDef != NULL)
			{
				string namespaceName = namespaceDef->get_namespaceDeclaration()->get_name();
				if (namespaceName == "gas_station" && className == "GasStationEvents")
				{
					string funcName = decl->get_name();

					if (funcName == "OnArrival" || funcName == "OnFinishedPumping" || funcName == "OnFinishedPaying")
						return true;
				}
			}
		}
		return false;
	}
};

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);
	AstTests::runAllTests(project);

	EventProcessor event_processor;

#ifdef REVERSE_CODE_GENERATION
	//Add the handlers in order of priority. The lower ones will be used only if higher ones do not produce results
	//Expression handlers:
	event_processor.addExpressionHandler(new IdentityExpressionHandler);
	event_processor.addExpressionHandler(new AkgulStyleExpressionHandler);
	event_processor.addExpressionHandler(new StoreAndRestoreExpressionHandler);

	//Statement handler
	event_processor.addStatementHandler(new ReturnStatementHandler);
	event_processor.addStatementHandler(new VariableDeclarationHandler);
	event_processor.addStatementHandler(new StraightlineStatementHandler);
	event_processor.addStatementHandler(new NullStatementHandler);

	//Variable value extraction handlers
	event_processor.addVariableValueRestorer(new RedefineValueRestorer);
	event_processor.addVariableValueRestorer(new ExtractFromUseValueRestorer);
#else
	event_processor.addStatementHandler(new StateSavingStatementHandler);
#endif

	SgScopeStatement* globalScope = isSgScopeStatement(SageInterface::getFirstGlobalScope(project));
	Backstroke::reverseEvents(&event_processor, IsEvent(globalScope), project);

	AstTests::runAllTests(project);
	return backend(project);
}
