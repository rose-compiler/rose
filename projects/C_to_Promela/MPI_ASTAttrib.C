#include "rose.h"
#include "iostream"

#include "MPI_ASTAttrib.h"

#define MPI_AST_ASTATTRIBSTR "CTP_MPI_AST_ATTRIB"

bool isNodeMPI(SgNode * node)
{
	MPI_ASTAttrib *attr=NULL;
	if (node->attributeExists(MPI_AST_ASTATTRIBSTR))
	{
		return true;
	}
	return false;
}

void markNodeAsMPI(SgNode * node)
{
	MPI_ASTAttrib *attr=NULL;
	if (node->attributeExists(MPI_AST_ASTATTRIBSTR))
	{
		
	}
	else
	{
		attr=new MPI_ASTAttrib();
	}
	node->setAttribute(MPI_AST_ASTATTRIBSTR,attr);
}


/*
#define C2PAST_VAR_STATE_ATTRIBSTR "VariableIsInPromelaState"

class C2PASTVarStateAttribute: public AstAttribute
{
	protected:
	bool inStateVector;
	bool globalState;
	public:
	C2PASTVarStateAttribute(){inStateVector=false;gobalState=false;}
	C2PASTVarStateAttribute(bool convert,bool global){inStateVector=convert;globalState=global};
	bool isInStateVector(){return inStateVector;};
	bool isGlobalState(){return globalState;};
	void setGlobal(bool val){globalState=val;};
	void setState(bool val){inStateVector=val;};
};*/
