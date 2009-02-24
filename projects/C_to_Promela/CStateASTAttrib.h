
#include "iostream"
#include "vector"
#define CSTATEASTATTRIBSTR "C_STATE_VECTOR"


class CStateASTAttrib: public AstAttribute
{

	protected:
	std::vector<SgInitializedName *> c_stateList;	
	public:
	CStateASTAttrib();
	void add(SgInitializedName* name);
	std::vector<SgInitializedName *> getIniNameList();
};
bool nodeHasCStateAttrib(SgNode*);
CStateASTAttrib * getCStateAttrib(SgNode*);
void setCStateAttrib(SgNode*,CStateASTAttrib*);
