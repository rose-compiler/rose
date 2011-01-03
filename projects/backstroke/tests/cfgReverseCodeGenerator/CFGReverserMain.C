#include <backstroke.h>
#include <VariableRenaming.h>
#include "utilities/cppDefinesAndNamespaces.h"
#include "normalizations/expNormalization.h"
#include "pluggableReverser/eventProcessor.h"
#include "pluggableReverser/expAndStmtHandlers.h"

using namespace std;

struct IsEvent
{
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

struct DummyNodeFilter : public std::unary_function<SgNode*, Rose_STL_Container<SgNode*> >
{
	Rose_STL_Container<SgNode*> operator()(SgNode* node) 
	{
		return Rose_STL_Container<SgNode*>(1, node);
	}
};

SgType* getPointerBaseType(SgType* type)
{
	if (isSgPointerType(type))
		return isSgPointerType(type)->get_base_type();
	else if (isSgModifierType(type))
		return getPointerBaseType(isSgModifierType(type)->get_base_type());
	else if (isSgTypedefType(type))
		return getPointerBaseType(isSgTypedefType(type)->get_base_type());
	else
		ROSE_ASSERT(false);
}

struct VariableReversalFilter : public IVariableFilter
{
	virtual bool isVariableInteresting(const VariableRenaming::VarName& var) const
	{
		SgType* type = var[0]->get_type();

		if (SageInterface::isPointerType(type))
			type = getPointerBaseType(type);

		string typeName = SageInterface::get_name(type);

		return (typeName != "DESEngine");
	}
};

int main(int argc, char** argv)
{
	//Add the preinclude option
	vector<string> commandArguments(argv, argv + argc);
	commandArguments.push_back("-include");
	commandArguments.push_back("rctypes.h");

	SgProject* project = frontend(commandArguments);
	AstTests::runAllTests(project);

	VariableReversalFilter varFilter;
	EventProcessor event_processor(&varFilter);

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

	Backstroke::reverseEvents(&event_processor, IsEvent(), project);

	//TODO: Fix the ROSE graphs so we don't have to do something this ugly
	//Hack x 10 to delete leftover objects from building the call graph
	VariantVector vv(V_SgDirectedGraphEdge);
	vv.push_back(V_SgGraphNode);
	vv.push_back(V_SgIncidenceDirectedGraph);
	Rose_STL_Container<SgNode*> edgeList = NodeQuery::queryMemoryPool(DummyNodeFilter(), &vv);
	foreach (SgNode* edge, edgeList)
	{
		delete edge;
	}

	AstTests::runAllTests(project);
	return backend(project);
}
