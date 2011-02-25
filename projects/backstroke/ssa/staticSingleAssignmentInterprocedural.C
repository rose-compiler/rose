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

void StaticSingleAssignment::interproceduralDefPropagation(const unordered_set<SgFunctionDefinition*>& interestingFunctions)
{
	ClassHierarchyWrapper* classHierarchy = new ClassHierarchyWrapper(project);
	vector<SgFunctionDefinition*> topologicalFunctionOrder = calculateInterproceduralProcessingOrder(interestingFunctions);

	//If there is no recursion, this should only requires one iteration. However, we have to always do an extra
	//iteration in which nothing changes
	int iteration = 0;
	while (true)
	{
		iteration++;
		bool changedDefs = false;
		foreach (SgFunctionDefinition* func, topologicalFunctionOrder)
		{
			bool newDefsForFunc = insertInterproceduralDefs(func, interestingFunctions, classHierarchy);
			changedDefs = changedDefs || newDefsForFunc;
		}

		if (!changedDefs)
			break;
	}
	if (getDebug())
		printf("%d interprocedural iterations on the call graph!\n", iteration);

	delete classHierarchy;
}


vector<SgFunctionDefinition*> StaticSingleAssignment::calculateInterproceduralProcessingOrder(
									const unordered_set<SgFunctionDefinition*>& interestingFunctions)
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

	//Order the functions of interest such that callees are processed before callers whenever possible
	vector<SgFunctionDefinition*> processingOrder;
	foreach (SgFunctionDefinition* interestingFunction, interestingFunctions)
	{
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


bool StaticSingleAssignment::insertInterproceduralDefs(SgFunctionDefinition* funcDef,
									const boost::unordered_set<SgFunctionDefinition*>& processed,
									ClassHierarchyWrapper* classHierarchy)
{
	vector<SgExpression*> functionCalls = SageInterface::querySubTree<SgExpression>(funcDef, V_SgFunctionCallExp);
	vector<SgExpression*> constructorCalls = SageInterface::querySubTree<SgExpression>(funcDef, V_SgConstructorInitializer);
	functionCalls.insert(functionCalls.end(), constructorCalls.begin(), constructorCalls.end());

	bool changedDefs = false;

	foreach(SgExpression* callSite, functionCalls)
	{
		//First, see which functions this call site leads too
		vector<SgFunctionDeclaration*> callees;
		CallTargetSet::getDeclarationsForExpression(callSite, classHierarchy, callees);

		LocalDefUseTable::mapped_type oldDefs = originalDefTable[callSite];

		//process each callee
		foreach(SgFunctionDeclaration* callee, callees)
		{
			processOneCallSite(callSite, callee, processed, classHierarchy);
		}

		const LocalDefUseTable::mapped_type& newDefs = originalDefTable[callSite];
		if (oldDefs != newDefs)
		{
			changedDefs = true;
		}
	}

	return changedDefs;
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
		varsDefinedinCallee = getOriginalVarsDefinedInSubtree(calleeDef);
	}
	else
	{
		//Nope, use an approximate bound :(
	}

	//Filter the variables that are not accessible from the caller and insert the rest as definitions
	foreach (const VarName& definedVar, varsDefinedinCallee)
	{
		if (isVarAccessibleFromCaller(definedVar, callSite, callee))
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
		VarName lhsVar = getVarForExpression(functionRefExpression->get_lhs_operand());

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
				//TODO: We can be more precise here! Instead of defining the lhsVar, we can find exactly which
				//elements of the lhs var were defined and only define those.
				//For example, obj.setX(3) should only have a def for obj.x rather than for all of obj.

				//Get the scope of variables in this class
				SgClassDefinition* calleeClassScope = calleeMemFunDecl->get_class_scope();
				ROSE_ASSERT(calleeClassScope != NULL);

				//If any of the callee's defined variables is a member variable, then the "this" instance has been modified
				foreach (const VarName& definedVar, varsDefinedinCallee)
				{
					//Only consider defs of member variables
					if (!varRequiresThisPointer(definedVar))
						continue;

					//If the modified var is in the callee class scope, we know "this" has been modified
					SgScopeStatement* varScope = SageInterface::getScope(definedVar[0]);
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

	//
	//Handle aliasing of parameters (e.g. parameters passed by reference)
	//

	//Get the actual arguments
	SgExprListExp* actualArguments = NULL;
	if (isSgFunctionCallExp(callSite))
		actualArguments = isSgFunctionCallExp(callSite)->get_args();
	else if (isSgConstructorInitializer(callSite))
		actualArguments = isSgConstructorInitializer(callSite)->get_args();
	ROSE_ASSERT(actualArguments != NULL);

	const SgExpressionPtrList& actualArgList = actualArguments->get_expressions();

	//Get the formal arguments. We use the defining declaration if possible, because that's the one that varNames are attached to
	SgInitializedNamePtrList formalArgList;
	if (calleeDef != NULL)
		formalArgList = calleeDef->get_declaration()->get_args();
	else
		formalArgList = callee->get_args();
	//The number of actual arguments can be less than the number of formal arguments (with implicit arguments) or greater
	//than the number of formal arguments (with varargs)

	//First, treat the true arguments
	for (size_t i = 0; i < actualArgList.size() && i < formalArgList.size(); i++)
	{
		//Check that the actual argument was a variable name
		const VarName& callerArgVarName = getVarForExpression(actualArgList[i]);
		if (callerArgVarName == emptyName)
			continue;

		//Check that the argument is passed by nonconst reference or is of a pointer type
		//Note: here we are also filtering varArg types (SgTypeEllipse)
		if (!isArgumentNonConstReferenceOrPointer(formalArgList[i]))
			continue;
		
		//See if we can use exact info here to determine if the callee modifies the argument
		//If not, we just take the safe assumption that the argument is modified
		bool argModified = true;
		if (calleeDef != NULL && processed.count(calleeDef) > 0)
		{
			//Get the variable name in the callee associated with the argument (since we've processed this function)
			const VarName& calleeArgVarName = getVarName(formalArgList[i]);

			ROSE_ASSERT(calleeArgVarName != emptyName);
			argModified = (varsDefinedinCallee.count(calleeArgVarName) > 0);
		}

		//Define the actual parameter in the caller if the callee modifies it
		if (argModified)
		{
			originalDefTable[callSite].insert(callerArgVarName);
		}
	}

	//Now, handle the implicit arguments. We can have an implicit argument such as (int& x = globalVar)
	//If there are more formal arguments than actual arguments there are two cases
	//case 1: The callee is a varArg function can no varargs are passed at the call site
	//case 2: The callee has default argument values passed in
	//We conly handle case 2. i.e. foo(int& x = globalVar)
	for (size_t i = actualArgList.size(); i < formalArgList.size(); i++)
	{
		SgInitializedName* formalArg = formalArgList[i];

		//Again, filter out parameters passed by value and const references
		if (!isArgumentNonConstReferenceOrPointer(formalArg))
			continue;

		//Default arguments always have an assign initializer
		if (formalArg->get_initializer() == NULL)
			continue;

		ROSE_ASSERT(isSgAssignInitializer(formalArg->get_initializer()));
		SgExpression* defaultArgValue = isSgAssignInitializer(formalArg->get_initializer())->get_operand();

		//See if the default value is a variable and that variable is in the caller's scope
		const VarName& defaultArgVar = getVarForExpression(defaultArgValue);
		if (defaultArgVar == emptyName || !isVarAccessibleFromCaller(defaultArgVar, callSite, callee))
			continue;

		//See if we can use exact info here to determine if the callee modifies the argument
		//If not, we just take the safe assumption that the argument is modified
		bool argModified = true;
		if (calleeDef != NULL && processed.count(calleeDef) > 0)
		{
			//Get the variable name in the callee associated with the argument (since we've processed this function)
			const VarName& calleeArgVarName = getVarName(formalArgList[i]);

			ROSE_ASSERT(calleeArgVarName != emptyName);
			argModified = (varsDefinedinCallee.count(calleeArgVarName) > 0);
		}

		//Define the default argument value in the caller if the callee modifies it
		if (argModified)
		{
			originalDefTable[callSite].insert(defaultArgVar);
		}
	}
}

bool StaticSingleAssignment::isVarAccessibleFromCaller(const VarName& var, SgExpression* callSite, SgFunctionDeclaration* callee)
{
	//If the variable modified in the callee is a member variable, see if it's on the same object instance
	if (varRequiresThisPointer(var))
	{
		ROSE_ASSERT(isSgMemberFunctionDeclaration(callee));

		//Constructor initializers always have a new object instance
		if (isSgConstructorInitializer(callSite))
			return false;

		if (!isThisPointerSameInCallee(isSgFunctionCallExp(callSite), isSgMemberFunctionDeclaration(callee)))
			return false;
	}

	return isVarInScope(var, callSite);
}


bool StaticSingleAssignment::varRequiresThisPointer(const StaticSingleAssignment::VarName& var)
{
	ROSE_ASSERT(var != emptyName);
	SgScopeStatement* varScope = SageInterface::getScope(var.front());

	SgClassDefinition* varClass = isSgClassDefinition(varScope);
	if (varClass == NULL)
		return false;

	//If it's a member variable, we still have to check that it's not static
	SgVariableDeclaration* varDecl = isSgVariableDeclaration(var.front()->get_declaration());
	ROSE_ASSERT(varDecl != NULL);

	if (varDecl->get_declarationModifier().get_storageModifier().isStatic())
		return false;

	//If it's a member variable and it's not static, requires implicit 'this' pointer
	return true;
}


bool StaticSingleAssignment::isThisPointerSameInCallee(SgFunctionCallExp* callSite, SgMemberFunctionDeclaration* callee)
{
	//We get the left-hand side of the call site. If it's the 'this' pointer, we should return true
	//E.g. should return true for this->bar(), but false for a.bar() because in the second case bar() acts on a different instance
	SgBinaryOp* functionRefExpression = isSgBinaryOp(callSite->get_function());
	ROSE_ASSERT(functionRefExpression != NULL);

	//The binary op is a dot op, arrow op, dot star op, etc
	return isThisPointer(functionRefExpression->get_lhs_operand());
}


bool StaticSingleAssignment::isThisPointer(SgExpression* expression)
{
	switch (expression->variantT())
	{
		case V_SgThisExp:
			return true;
		case V_SgCastExp:
		case V_SgPointerDerefExp:
		case V_SgAddressOfOp:
		{
			SgUnaryOp* op = isSgUnaryOp(expression);
			ROSE_ASSERT(op != NULL);
			return isThisPointer(op->get_operand());
		}
		case V_SgCommaOpExp:
			return isThisPointer(isSgCommaOpExp(expression)->get_rhs_operand());
		default:
			return false;
	}
}

//! True if the type is a pointer pointing to a const object.
//! expanded recursively.
bool StaticSingleAssignment::isPointerToDeepConst(SgType* type)
{
	if (SgTypedefType* typeDef = isSgTypedefType(type))
	{
		return isPointerToDeepConst(typeDef->get_base_type());
	}
	else if (SgPointerType* pointerType = isSgPointerType(type))
	{
		SgType* baseType = pointerType->get_base_type();
		if (SageInterface::isPointerType(baseType))
		{
			return isDeepConstPointer(baseType);
		}
		else
		{
			return SageInterface::isConstType(pointerType->get_base_type());
		}
	}
	else if (SgModifierType* modifierType = isSgModifierType(type))
	{
		//We don't care about qualifiers of the top-level type. Only the object it points to must be
		//const
		return isPointerToDeepConst(modifierType->get_base_type());
	}
	else
	{
		return false;
	}
}

//! True if the type is a const pointer pointing to a const object.
//! Expanded recursively
bool StaticSingleAssignment::isDeepConstPointer(SgType* type)
{
	if (SgTypedefType* typeDef = isSgTypedefType(type))
	{
		return isDeepConstPointer(typeDef->get_base_type());
	}
	else if (SgModifierType* modifierType = isSgModifierType(type))
	{
		bool isConst = modifierType->get_typeModifier().get_constVolatileModifier().isConst();

		if (isConst)
		{
			//If this pointer is const, we still have to make sure it points to a const value
			return isPointerToDeepConst(modifierType->get_base_type());
		}
		else
		{
			//This was not a const-qualifier, so it changes nothing
			return isDeepConstPointer(modifierType->get_base_type());
		}
	}
	else
	{
		return false;
	}
}

bool StaticSingleAssignment::isArgumentNonConstReferenceOrPointer(SgInitializedName* formalArgument)
{
	SgType* formalArgType = formalArgument->get_type();
	if (SageInterface::isNonconstReference(formalArgType))
		return true;

	if (SageInterface::isPointerType(formalArgType))
		return !isPointerToDeepConst(formalArgType);

	return false;
}