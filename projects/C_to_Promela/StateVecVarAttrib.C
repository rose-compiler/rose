#include "rose.h"
#include "iostream"
#include "vector"
#include "StateVecVarAttrib.h"


bool nodeHasStateVecVarAttrib(SgNode* node)
{
	return node->attributeExists(STATEVECVARASTATTRIBSTR);
}
StateVecVarASTAttrib * getStateVecVarAttrib(SgNode* node)
{
	if (nodeHasStateVecVarAttrib(node))
	{
		return dynamic_cast<StateVecVarASTAttrib*>(node->getAttribute(STATEVECVARASTATTRIBSTR));
	}
	else return NULL;
}
void setStateVecVarAttrib(SgNode*node,StateVecVarASTAttrib* attr)
{
	node->setAttribute(STATEVECVARASTATTRIBSTR,attr);
}
