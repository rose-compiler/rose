#include "rose.h"
#include "testDefUse.h"
int main(int argc,char**argv) {
	SgProject* proj = frontend(argc,argv);
	ROSE_ASSERT(proj != NULL);
	initDefUseAnalysis(proj);
	// loop over vars..
	NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(proj,V_SgVarRefExp);
	NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
	for (; i != vars.end(); ++i) {
		debugAnalyzeVariableReference(isSgVarRefExp(*i));
	}
	return 0;
}	


