#if !defined(FUNCTION_STATE_H) && !defined(NO_FUNCTION_STATE_H)
#define FUNCTION_STATE_H

#include "analysisCommon.h"
#include "nodeState.h"
#include "CallGraphTraverse.h"
#include <set>

using namespace std;

class FunctionState
{
	friend class CollectFunctions;
	public:
	Function func;
	NodeState state;

	private:
	static set<FunctionState*> allDefinedFuncs;	
	static set<FunctionState*> allFuncs;
	static bool allFuncsComputed;
		
	public:
	FunctionState(Function &func): 
		func(func),
		state(/*func.get_declaration()->cfgForBeginning()*/)
	{}
	
	Function& getFunc();
	
	// returns a set of all the functions whose bodies are in the project
	static set<FunctionState*>& getAllDefinedFuncs();
	
	// returns a set of all the functions whose declarations are in the project
	static set<FunctionState*>& getAllFuncs();
	
	// returns the FunctionState associated with the given function
	// func may be any defined function
	static FunctionState* getDefinedFuncState(const Function& func);
	
	// returns the FunctionState associated with the given function
	// func may be any declared function
	static FunctionState* getFuncState(const Function& func);
	
	// given a function call, sets argParamMap to map all simple arguments to this function to their 
	// corresponding parameters
	static void setArgParamMap(SgFunctionCallExp* call, map<varID, varID>& argParamMap);
	
	// given a function call, sets argParamMap to map all the parameters of this function to their 
	// corresponding simple arguments, if those arguments are passed by reference
	static void setParamArgByRefMap(SgFunctionCallExp* call, map<varID, varID>& paramArgByRefMap);
};

class CollectFunctions : public TraverseCallGraphBottomUp<int>
{
	public:
	CollectFunctions(SgIncidenceDirectedGraph* graph) : TraverseCallGraphBottomUp<int>(graph)
	{}
	
	int visit(const CGFunction* func, list<int> fromCallees);
};

#endif
