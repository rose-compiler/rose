//Author: George Vulov <georgevulov@hotmail.com>

/** Here we put all the functions related to interprocedural analysis. Interprocedural analysis concerns itself with what
 * variables are defined at function call expressions. If interprocedural analysis is turned off, function call expressions
 * never have definitions of variables. */

#include "rose.h"
#include "CallGraph.h"
#include "staticSingleAssignment.h"

#define foreach BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

using namespace std;
using namespace ssa_private;
using namespace boost;


vector<SgFunctionDefinition*> StaticSingleAssignment::calculateInterproceduralProcessingOrder()
{
	//First, let's build a call graph. Our goal is to find an order in which to process the functions
	//So that callees are processed before callers. This way we would have exact information at each call site
	CallGraphBuilder cgBuilder(project);
	FunctionFilter functionFilter;
	cgBuilder.buildCallGraph(functionFilter);

	SgIncidenceDirectedGraph* callGraph = cgBuilder.getGraph();

	//Build a map from SgGraphNode* to the corresponding function definitions
	unordered_map<SgFunctionDefinition*, SgGraphNode*> graphNodeToFunction;
	set<SgGraphNode*> allNodes = callGraph->computeNodeSet();

	foreach(SgGraphNode* graphNode, allNodes)
	{
		SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(graphNode->get_SgNode());
		ROSE_ASSERT(funcDecl != NULL);
		funcDecl = isSgFunctionDeclaration(funcDecl->get_definingDeclaration());
		ROSE_ASSERT(funcDecl != NULL);

		SgFunctionDefinition* funcDef = funcDecl->get_definition();
		graphNodeToFunction[funcDef] = graphNode;
	}

	//Find functions of interest
	vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition> (project, V_SgFunctionDefinition);

	//Order the functions of interest such that callees are processed before callers whenever possible
	vector<SgFunctionDefinition*> processingOrder;
	foreach (SgFunctionDefinition* interestingFunction, funcs)
	{
		if (functionFilter(interestingFunction->get_declaration()))
			processCalleesThenFunction(interestingFunction, callGraph, graphNodeToFunction, processingOrder);
	}

	return processingOrder;
}


void StaticSingleAssignment::processCalleesThenFunction(SgFunctionDefinition* targetFunction, SgIncidenceDirectedGraph* callGraph,
		unordered_map<SgFunctionDefinition*, SgGraphNode*> graphNodeToFunction,
		vector<SgFunctionDefinition*> &processingOrder)
{
	//If the function is already in the list of functions to be processed, don't add it again.
	if (find(processingOrder.begin(), processingOrder.end(), targetFunction) != processingOrder.end())
		return;

	if (graphNodeToFunction.count(targetFunction) == 0)
	{
		printf("The function %s has no vertex in the call graph!\n", targetFunction->get_declaration()->get_name().str());
		ROSE_ASSERT(false);
	}

	SgGraphNode* graphNode = graphNodeToFunction[targetFunction];

	vector<SgGraphNode*> callees;
	callGraph->getSuccessors(graphNode, callees);

	//Recursively process all the callees before adding this function to the list
	foreach(SgGraphNode* callerNode, callees)
	{
		SgFunctionDeclaration* callerDecl = isSgFunctionDeclaration(callerNode->get_SgNode());
		ROSE_ASSERT(callerDecl != NULL);
		callerDecl = isSgFunctionDeclaration(callerDecl->get_definingDeclaration());
		ROSE_ASSERT(callerDecl != NULL);
		SgFunctionDefinition* callee = callerDecl->get_definition();

		processCalleesThenFunction(callee, callGraph, graphNodeToFunction, processingOrder);
	}

	//If the function is already in the list of functions to be processed, don't add it again.
	//Some of our callees might have added us due to recursion
	if (find(processingOrder.begin(), processingOrder.end(), targetFunction) == processingOrder.end())
		processingOrder.push_back(targetFunction);
}


void StaticSingleAssignment::insertInterproceduralDefs(SgFunctionDefinition* funcDef,
									const boost::unordered_set<SgFunctionDefinition*>& processed,
									ClassHierarchyWrapper* classHierarchy)
{
	vector<SgFunctionCallExp*> functionCalls = SageInterface::querySubTree<SgFunctionCallExp>(funcDef, V_SgFunctionCallExp);

	foreach(SgFunctionCallExp* callSite, functionCalls)
	{
		//First, see which functions this call site leads too
		vector<SgFunctionDeclaration*> callees;
		CallTargetSet::getDeclarationsForExpression(callSite, classHierarchy, callees);

		//Here we store all variables defined at the calls site
		set<VarName> modifiedVars;

		foreach(SgFunctionDeclaration* callee, callees)
		{
			SgFunctionDefinition* calleeDef = NULL;
			if (callee->get_definingDeclaration() != NULL)
			{
				calleeDef = isSgFunctionDeclaration(callee->get_definingDeclaration())->get_definition();
				if (calleeDef == NULL)
				{
					fprintf(stderr, "WARNING: Working around a ROSE bug. The function %s\n", callee->get_name().str());
					fprintf(stderr, "has a defining declaration but no definition!");
					continue;
				}
			}

			//See if we can get exact information because the function has already been processed
			if (calleeDef != NULL && processed.count(calleeDef) > 0)
			{
				//Yes, use exact info!
				set<VarName> calleeModifiedVars = getExactInterproceduralDefs(callSite, calleeDef);
				modifiedVars.insert(calleeModifiedVars.begin(), calleeModifiedVars.end());
			}
			else
			{
				//Nope, use an approximate bound :(

			}

			//Check if this is a member function. In this case, we should check if the "this" instance is modified
			if (isSgMemberFunctionDeclaration(callee))
			{
				//TODO
			}
		}

		//Insert the interprocedural defs at the call site
		originalDefTable[callSite].insert(modifiedVars.begin(), modifiedVars.end());
	}
}


set<StaticSingleAssignment::VarName> StaticSingleAssignment::getExactInterproceduralDefs(SgFunctionCallExp* callSite,
									SgFunctionDefinition* callee)
{
	class CollectDefsTraversal : public AstSimpleProcessing
	{
	public:
		StaticSingleAssignment* ssa;

		//All the varNames that have uses in the function
		set<VarName> definedNames;

		void visit(SgNode* node)
		{
			if (ssa->ssaLocalDefTable.find(node) == ssa->ssaLocalDefTable.end())
				return;

			NodeReachingDefTable& nodeDefs = ssa->ssaLocalDefTable[node];

			foreach(const NodeReachingDefTable::value_type& varDefPair, nodeDefs)
			{
				definedNames.insert(varDefPair.first);
			}
		}
	};

	CollectDefsTraversal defsTrav;
	defsTrav.ssa = this;
	defsTrav.traverse(callee, preorder);

	set<VarName> defs;

	//Filter the variables that are not accessible from the caller
	foreach (const VarName& definedVar, defsTrav.definedNames)
	{
		if (isVarInScope(definedVar, callSite))
			defs.insert(definedVar);
	}

	//Add all variables that are aliased through function arguments
	//E.g. variables passed by reference at the call site
	//TODO:

	return defs;
}