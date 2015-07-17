#include "testVariableFinalStateCalc.h"


int main(int argc, char** argv) {
	SgProject* proj = frontend(argc,argv);
	initDefUseAnalysis(proj);
	NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(proj,V_SgVarRefExp);
	NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
	initializeScopeInformation(proj);
	for (; i != vars.end(); i++) {
		SgVarRefExp* varI = isSgVarRefExp(*i);
		std::string stringout = getVariableFinalState(varI);
		std::cout << "variable final state" << varI->get_symbol()->get_name() << ":" << std::endl;
		std::cout << stringout << std::endl;
	}
	backend(proj);
	return 0;
}
