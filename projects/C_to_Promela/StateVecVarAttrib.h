
#include "iostream"
#include "vector"

// StateVecVarAttrib
#define STATEVECVARASTATTRIBSTR "STATE_VEC_VAR_ATTRIB"


class StateVecVarASTAttrib: public AstAttribute
{

	protected:
		bool varIsLocal;
		bool varIsInStateVector;	
	public:
	StateVecVarASTAttrib(bool local):varIsLocal(local){
		varIsInStateVector=true;
	}
	void setInState(bool val){varIsInStateVector=val;};
	void setGlobal(bool val){varIsLocal=!val;};
	void markAsGlobal(){varIsLocal=false;};
	void setNotInState()
	{
		varIsInStateVector=false;
	}
	bool isInState()
	{
		return varIsInStateVector;	
	}
	bool isLocalState()
	{
		return varIsInStateVector && varIsLocal;
	}
	bool isGlobalState()
	{
		return varIsInStateVector && !varIsLocal;
	}
	
	bool isLocalVar()
	{
		return varIsLocal;
	}
	bool isGlobalVar()
	{
		return !varIsLocal;
	}
};

bool nodeHasStateVecVarAttrib(SgNode* node);
StateVecVarASTAttrib * getStateVecVarAttrib(SgNode* node);
void setStateVecVarAttrib(SgNode*node,StateVecVarASTAttrib* attr);
