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
	vector<SgExpression*> functionCalls = SageInterface::querySubTree<SgExpression>(funcDef, V_SgFunctionCallExp);
	vector<SgExpression*> constructorCalls = SageInterface::querySubTree<SgExpression>(funcDef, V_SgConstructorInitializer);
	functionCalls.insert(functionCalls.end(), constructorCalls.begin(), constructorCalls.end());

	foreach(SgExpression* callSite, functionCalls)
	{
		//First, see which functions this call site leads too
		vector<SgFunctionDeclaration*> callees;
		CallTargetSet::getDeclarationsForExpression(callSite, classHierarchy, callees);

		foreach(SgFunctionDeclaration* callee, callees)
		{
			processOneCallSite(callSite, callee, processed, classHierarchy);
		}
	}
}


void StaticSingleAssignment::processOneCallSite(SgExpression* callSite, SgFunctionDeclaration* callee,
							const unordered_set<SgFunctionDefinition*>& processed, ClassHierarchyWrapper* classHierarchy)
{
	ROSE_ASSERT(isSgFunctionCallExp(callSite) || isSgConstructorInitializer(callSite));
	SgFunctionDefinition* calleeDef = NULL;
	if (callee->get_definingDeclaration() != NULL)
	{
		calleeDef = isSgFunctionDeclaration(callee->get_definingDeclaration())->get_definition();
		if (calleeDef == NULL)
		{
			fprintf(stderr, "WARNING: Working around a ROSE bug. The function %s\n", callee->get_name().str());
			fprintf(stderr, "has a defining declaration but no definition!");
		}
	}

	//See if we can get exact information because the function has already been processed
	set<VarName> varsDefinedinCallee;
	if (calleeDef != NULL && processed.count(calleeDef) > 0)
	{
		//Yes, use exact info!
		varsDefinedinCallee = getVarsDefinedInSubtree(calleeDef);
	}
	else
	{
		//Nope, use an approximate bound :(
	}

	//Filter the variables that are not accessible from the caller and insert the rest as definitions
	foreach (const VarName& definedVar, varsDefinedinCallee)
	{
		if (isVarInScope(definedVar, callSite))
			originalDefTable[callSite].insert(definedVar);
	}

	//Check if this is a member function. In this case, we should check if the "this" instance is modified
	SgMemberFunctionDeclaration* calleeMemFunDecl = isSgMemberFunctionDeclaration(callee);
	if (calleeMemFunDecl != NULL
			&& !calleeMemFunDecl->get_declarationModifier().get_storageModifier().isStatic()
			&& isSgFunctionCallExp(callSite)) //No need to consider constructor initializers here, because they don't have a LHS var
	{
		//Get the LHS variable (e.g. x in the call site x.foo())
		SgBinaryOp* functionRefExpression = isSgBinaryOp(isSgFunctionCallExp(callSite)->get_function());
		ROSE_ASSERT(functionRefExpression != NULL);
		VarName lhsVar = getVarName(functionRefExpression->get_lhs_operand());

		//It's possible that the member function is not operating on a variable; e.g. the function bar in foo().bar()
		if (lhsVar != emptyName)
		{
			//If the callee has no definition, then we assume it modifies the object unless it is declared const
			//This is also our loose estimate in case there is recursion
			if (calleeDef == NULL || processed.count(calleeDef) == 0)
			{
				SgMemberFunctionType* calleeFuncType = isSgMemberFunctionType(calleeMemFunDecl->get_type());
				ROSE_ASSERT(calleeFuncType != NULL);
				if (!calleeFuncType->isConstFunc())
				{
					originalDefTable[callSite].insert(lhsVar);
				}
			}
			//If the callee has a definition and we have already processed it we can use exact info to check if 'this' is modified
			else
			{
				//Get the scope of variables in this class
				SgClassDefinition* calleeClassScope = calleeMemFunDecl->get_class_scope();
				ROSE_ASSERT(calleeClassScope != NULL);

				//If any of the callee's defined variables is a member variable, then the "this" instance has been modified
				foreach (const VarName& definedVar, varsDefinedinCallee)
				{
					//Only consider defs of member variables
					SgScopeStatement* varScope = SageInterface::getScope(definedVar[0]);
					if (!isSgClassDefinition(varScope))
						continue;

					//Only consider static variables
					SgVariableDeclaration* varDecl = isSgVariableDeclaration(definedVar[0]->get_parent());
					ROSE_ASSERT(varDecl != NULL);
					if (varDecl->get_declarationModifier().get_storageModifier().isStatic())
						continue;

					//If the modified var is in the callee class scope, we know "this" has been modified
					if (varScope == calleeClassScope)
					{
						originalDefTable[callSite].insert(lhsVar);
						break;
					}

					//Even if the modified var is not in the callee's class scope, it could be an inherited variable
					Rose_STL_Container<SgClassDefinition*> superclasses = classHierarchy->getAncestorClasses(calleeClassScope);
					if (find(superclasses.begin(), superclasses.end(), varScope) != superclasses.end())
					{
						originalDefTable[callSite].insert(lhsVar);
						break;
					}
				}
			}
		}
	}

	//Last thing: handle parameters passed by reference

}
