#include "rose.h"

int main(int argc, char** argv) {
	SgProject* proj = frontend(argc,argv);
	SgFunctionDeclaration* mainDecl = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDef = mainDecl->get_definition();
	std::vector<SgNode*> unaryOpNodes = NodeQuery::querySubTree(mainDef,V_SgUnaryOp);
	for (int i = 0; i < unaryOpNodes.size(); i++) {
		std::cout << unaryOpNodes[i]->class_name() << std::endl;
		SgUnaryOp::Sgop_mode m  = isSgUnaryOp(unaryOpNodes[i])->get_mode();	
			
		if (m == 0) {
			std::cout << "prefix" << std::endl;
		}
		else if (m == 1) { 
			std::cout << "postfix" << std::endl;
		}
		else {
			std::cout << "Neither post nor prefix" << std::endl; 
		}
	}
	return 0;
}		

