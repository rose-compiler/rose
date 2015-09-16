#include <iostream>
#include "rose.h"

int main(int argc, char** argv) {
	SgProject* proj = frontend(argc,argv);
	SgFunctionDeclaration* mainDecl = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDef = mainDecl->get_definition();
	std::vector<SgNode*> functionCalls = NodeQuery::querySubTree(mainDef, V_SgFunctionCallExp);
	for (int i = 0; i < functionCalls.size(); i++) {
		SgFunctionDeclaration* funcDecl = isSgFunctionCallExp(functionCalls[i])->getAssociatedFunctionDeclaration();
		SgFunctionParameterList* lst = funcDecl->get_parameterList();
		SgInitializedNamePtrList ptr_lst = funcDecl->get_args();
		SgType* retType = funcDecl->get_orig_return_type();
		SgInitializedNamePtrList::iterator j = ptr_lst.begin();
		std::cout << "f : " << funcDecl->get_name().getString() << std::endl;
		std::cout << "args: " << std::endl;
		while (j != ptr_lst.end()) {
			SgType* argTyp = (*j)->get_type();
			std::cout << argTyp->class_name() << std::endl;
			j++;
		}
		std::cout << "retType: " << retType->class_name() << std::endl;
	}
	return 0;
}
	
	
