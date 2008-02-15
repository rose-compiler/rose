#include "rose.h"
#include "iostream"
#include "vector"
#include "CStateASTAttrib.h"

CStateASTAttrib::CStateASTAttrib()
{
};
void CStateASTAttrib::add(SgInitializedName* name)
{
	c_stateList.push_back(name);
};
std::vector<SgInitializedName *> CStateASTAttrib::getIniNameList()
{
	return c_stateList;
};

bool nodeHasCStateAttrib(SgNode*node)
{
	return node->attributeExists(CSTATEASTATTRIBSTR);
}
CStateASTAttrib * getCStateAttrib(SgNode*node)
{
	
	if (nodeHasCStateAttrib(node))
	{
		return dynamic_cast<CStateASTAttrib*>(node->getAttribute(CSTATEASTATTRIBSTR));
	}
	else
	{
		return NULL;
	}
}

void setCStateAttrib(SgNode * node,CStateASTAttrib*data)
{
	node->setAttribute(CSTATEASTATTRIBSTR,data);
}

