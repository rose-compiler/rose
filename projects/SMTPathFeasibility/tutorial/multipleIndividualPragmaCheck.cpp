//typedef std::pair<SgPragmaDeclaration*, SgPragmaDeclaration*> pragma_pair
#include "multipleIndividualPragmaCheck.h"//sixPragmaCheckSMTTraversal.h"//"twoPragmaCheckSMTTraversal.h"
int main(int argc, char* argv[]) {
	SgProject* proj = frontend(argc,argv);
	//initGraph(proj);
	disp_header();
	std::vector<std::string> pr_names;
	pr_names.push_back("p1_right");
	pr_names.push_back("p1_wrong");
	pr_names.push_back("p2_right");
	pr_names.push_back("p2_wrong");
	pr_names.push_back("p3_right");
	pr_names.push_back("p3_wrong");
	for (int i = 0; i < 6; i++) {
	std::cout << "i: " << i << std::endl;
	//std::cout << "( push )" << std::endl;
	std::cout << "get_named_pragma" << std::endl;
	pragma_pair my_pragma_pair = get_named_pragma(proj,pr_names[i]);
	std::cout << "pair: " << my_pragma_pair.first->get_pragma()->get_pragma() << ", " << my_pragma_pair.second->get_pragma()->get_pragma() << std::endl;
	std::cout << "finding pragmas" << std::endl;
	findPragmaPaths(my_pragma_pair,proj);
	std::cout << "found pragmas" << std::endl;
	//std::cout << "( pop )" << std::endl;
	}
	std::cout << ";; traversal completed" << std::endl;
	return 0;
}		
