#include <rose.h>

#include <iostream>

#include "cuda_traversal.hpp"

int main(int argc, char ** argv) {

	ROSE_ASSERT(argc == 2);

	SgProject* project = frontend(argc, argv);
	ROSE_ASSERT(project != NULL);

	CudaTraversal test;
	
	test.traverseInputFiles(project, preorder);
	
	project->unparse();

	return 0;
}
