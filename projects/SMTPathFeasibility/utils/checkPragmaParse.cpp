#include "checkPragmaParse.h" 

int main(int argc, char* argv[]) {
	std::vector<std::string> pragma_values;
	SgProject* proj = frontend(argc,argv);
	ROSE_ASSERT(proj != NULL);
	SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
	
	Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(proj,V_SgPragmaDeclaration);
	for (Rose_STL_Container<SgNode*>::iterator i = pragmaList.begin(); i != pragmaList.end(); i++) {
		SgPragmaDeclaration* prg = isSgPragmaDeclaration(*i);
		std::string prag_string = get_pragma_string(prg);
		if (isSMTGeneral(prag_string)) {
			std::string parsed = parseSMTGeneral(prag_string);
			std::cout << ";;SMTGeneral command found: " << parsed << std::endl;
			//continue;
		}
		else {
		//std::cout << "SMTGeneral command not found" << std::endl;
		std::string pos;
		pos = get_position(prag_string);
		if (pos == "define") {
			std::cout << "define:\n";
			std::vector<std::pair<std::string, std::string> > pragma_values = get_vars(prag_string);
			for (std::vector<std::pair<std::string,std::string> >::iterator j = pragma_values.begin(); j != pragma_values.end(); j++) {
				std::cout << (*j).first << " " << (*j).second << std::endl;
			}
			
		}
		else if (pos == "assume") {
			std::cout << "assume:\n";
			std::vector<std::string> assumptions = get_assumptions(prag_string);
			for (std::vector<std::string>::iterator i = assumptions.begin(); i != assumptions.end(); i++) {
				std::cout << (*i) << std::endl;
			}	
		}
		else if (pos == "begin" || pos == "end") {
			std::cout << "position: " << pos << std::endl;
		}
		else {
			ROSE_ASSERT(false);
		}
		}
	}
	return 0;
}	
			

