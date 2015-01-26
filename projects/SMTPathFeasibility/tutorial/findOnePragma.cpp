//typedef std::pair<SgPragmaDeclaration*, SgPragmaDeclaration*> pragma_pair
#include "findOnePragma.h"

int main(int argc, char* argv[]) {
	SgProject* proj = frontend(argc,argv);
	//initGraph(proj);
	
	pragma_pair my_pragma_pair = get_named_pragma(proj,"my_pragma");
	ROSE_ASSERT(my_pragma_pair.first != NULL);
	ROSE_ASSERT(my_pragma_pair.second != NULL);
	std::cout << "findingprags" << std::endl;
	findPragmaPaths(my_pragma_pair,proj);
	std::cout << "traversal complete" << std::endl;
	return 0;
}		
