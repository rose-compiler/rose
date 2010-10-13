#include "functionState.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.C"

/*********************
 *** FunctionState ***
 *********************/

Function& FunctionState::getFunc()
{
	return func;
}

set<FunctionState*> FunctionState::allDefinedFuncs;	
set<FunctionState*> FunctionState::allFuncs;
bool FunctionState::allFuncsComputed=false;
	
// returns a set of all the functions whose bodies are in the project
set<FunctionState*>& FunctionState::getAllDefinedFuncs()
{
	if(allFuncsComputed)
		return allDefinedFuncs;
	else
	{
		CollectFunctions collect(getCallGraph());
		collect.traverse();
		allFuncsComputed=true;
		return allFuncs;
	}
}

// returns a set of all the functions whose declarations are in the project
set<FunctionState*>& FunctionState::getAllFuncs()
{
	if(allFuncsComputed)
		return allFuncs;
	else
	{
		CollectFunctions collect(getCallGraph());
		collect.traverse();
		allFuncsComputed=true;
	}
}

// returns the FunctionState associated with the given function
// func may be any defined function
FunctionState* FunctionState::getDefinedFuncState(const Function& func)
{
	for(set<FunctionState*>::iterator it=allDefinedFuncs.begin(); it!=allDefinedFuncs.end(); it++)	
		if((*it)->func == func)
			return *it;
	return NULL;
}

// returns the FunctionState associated with the given function
// func may be any declared function
FunctionState* FunctionState::getFuncState(const Function& func)
{
	for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)	
		if((*it)->func == func)
			return *it;
	return NULL;
}

// given a function call, sets argParamMap to map all simple arguments to this function to their 
// corresponding parameters
void FunctionState::setArgParamMap(SgFunctionCallExp* call, map<varID, varID>& argParamMap)
{
	Function func(call);
	
	SgExpressionPtrList args = call->get_args()->get_expressions();
	//SgInitializedNamePtrList params = funcArgToParamByRef(call);
	SgInitializedNamePtrList params = func.get_params();
	ROSE_ASSERT(args.size() == params.size());
	
	// the state of the callee's variables at the call site
	SgExpressionPtrList::iterator itA;
	SgInitializedNamePtrList::iterator itP;
	for(itA = args.begin(), itP = params.begin(); 
	    itA!=args.end() && itP!=params.end(); 
	    itA++, itP++)
	{
		// if this argument is a simple variable reference
		if(varID::isValidVarExp(*itA))
		{
			varID arg(*itA);
			varID param(*itP);
			argParamMap[arg] = param;
		}
	}
}
	
// given a function call, sets argParamMap to map all the parameters of this function to their 
// corresponding simple arguments, if those arguments are passed by reference
void FunctionState::setParamArgByRefMap(SgFunctionCallExp* call, map<varID, varID>& paramArgByRefMap)
{
	Function func(call);

	SgExpressionPtrList args = call->get_args()->get_expressions();	
	SgInitializedNamePtrList params = func.get_params();
	
	SgExpressionPtrList::iterator itArgs;
	SgInitializedNamePtrList::iterator itParams;
	for(itParams = params.begin(), itArgs = args.begin(); 
	    itParams!=params.end() && itArgs!=args.end(); 
	    itParams++, itArgs++)
	{
		SgType* typeParam = (*itParams)->get_type();
		SgType* typeArg = cfgUtils::unwrapCasts((*itArgs))->get_type();
			
		/*printf("FunctionState::setParamArgByRefMap() *itArgs=<%s | %s>\n", (*itArgs)->unparseToString().c_str(), (*itArgs)->class_name().c_str());
		printf("                                     typeArg=<%s | %s>\n", typeArg->unparseToString().c_str(), typeArg->class_name().c_str());
		printf("                                     itParams=<%s | %s>\n", (*itParams)->unparseToString().c_str(), (*itParams)->class_name().c_str());
		printf("                                     typeParam=<%s | %s>\n", typeParam->unparseToString().c_str(), typeParam->class_name().c_str());*/
		
			// if the argument is a named variable AND
		if(varID::isValidVarExp(*itArgs) && 
			 // if the argument has an array type, it's contents will be passed by reference OR 
			((isSgArrayType(typeArg) || isSgPointerType(typeArg)) ||
			 // if the argument is a regular variable, by the parameter has a reference type, 
			 // or the variable is being passed via a pointer, the variable must be passed by reference
			 isSgReferenceType(typeParam)))
		{
			varID argVar(*itArgs);
			varID paramVar(*itParams);
			
			// add this mapping
			paramArgByRefMap[paramVar] = argVar;
		}
		/*
		// if the argument is a non-array variable being passed via a pointer
		else if(isSgAddressOfOp(*itArgs) && SgPointerType(typeArg) && 
			     SgPointerType(typeParam)*/
	}
}


/************************
 *** CollectFunctions ***
 ************************/

int CollectFunctions::visit(const CGFunction* cgFunc, list<int> fromCallees)
{
	Function func(cgFunc);
	FunctionState* fs = new FunctionState(func);
	
	//printf("CollectFunctions::visit func = %s\n", func.get_name().str());
	
	// if the function has a body
	if(func.get_definition())
		FunctionState::allDefinedFuncs.insert(fs);
	FunctionState::allFuncs.insert(fs);
	return 0;
}
