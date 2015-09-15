#include "rose.h"

int main(int argc, char* argv[]) {
	SgProject* proj = frontend(argc,argv);
	SgFunctionDeclaration* mainDecl = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDef = mainDecl->get_definition();
//	std::vector<SgNode*> dotExps = NodeQuery::querySubTree(mainDef, V_SgDotExp);
	std::vector<SgNode*> varRefs = NodeQuery::querySubTree(mainDef,V_SgVarRefExp);
	int classExps = 0;
	for (unsigned int i = 0; i < varRefs.size(); i++) {
		if (isSgClassType(isSgVarRefExp(varRefs[i])->get_type())) {
			SgClassType* ct = isSgClassType(isSgVarRefExp(varRefs[i])->get_type());
			std::cout << "name of ref: " << isSgVarRefExp(varRefs[i])->get_symbol()->get_name().getString() << std::endl;
			if (SageInterface::isStructType(ct)) {
			SgDeclarationStatement* decl = isSgType(ct)->getAssociatedDeclaration();
			SgDeclarationStatement* defining_decl = decl->get_definingDeclaration();
			if (!(defining_decl->isNameOnly())) {	
				if (isSgClassDeclaration(defining_decl)) {
				if (isSgClassDeclaration(defining_decl)->get_definition()) {
				SgDeclarationStatementPtrList member_stats = isSgClassDeclaration(defining_decl)->get_definition()->get_members();
				SgDeclarationStatementPtrList::iterator j = member_stats.begin();
				for (; j != member_stats.end(); j++) {
					SgDeclarationStatement* d = isSgDeclarationStatement(*j);
					std::cout << "decl stat name: " << d->class_name() << std::endl;
					SgInitializedNamePtrList init_lst = isSgVariableDeclaration(d)->get_variables();
					SgInitializedNamePtrList::iterator k = init_lst.begin();
					std::cout << "variables in initialized name ptr list..." << std::endl;
					for (; k != init_lst.end(); k++) {
						std::cout << isSgInitializedName(*k)->get_name().getString() << std::endl;
						std::cout << isSgInitializedName(*k)->get_type()->class_name() << std::endl;
					}
				}
					
				classExps+=1;
				}
				}
			}
			
			
			}
		}
	}	
	std::cout << "num class_exp: " << classExps << std::endl;
	return 0;
}	
