// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include "cfgToDot.h"
#include "virtualCFG.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;
using namespace VirtualCFG;

int main( int argc, char * argv[] ) 
{
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);
	
	generatePDF ( *project );
	
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i) {
		SgFunctionDefinition* curFunc = isSgFunctionDefinition(*i);
		ROSE_ASSERT(curFunc);
				
		SgBasicBlock *funcBody = curFunc->get_body();
		InterestingNode funcCFGStart = (InterestingNode)funcBody->cfgForBeginning();;
			
		// output the CFG to a file
		ofstream fileCFG;
		fileCFG.open((curFunc->get_declaration()->get_name().getString()+"_cfg.dot").c_str());
		cout << "writing to file "<<(curFunc->get_declaration()->get_name().getString()+"_cfg.dot")<<"\n";
		VirtualCFG::cfgToDot(fileCFG, curFunc->get_declaration()->get_name(), funcCFGStart);
		fileCFG.close();
	}
	
	// Unparse and compile the project (so this can be used for testing)
	return backend(project);
}
