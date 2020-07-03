// Include header file the first time to define class Simple.
#include "../Simple.h"

// Set a macro we will use to take a different path in the same header file.
#define ALTERNATIVE 1

// Include the same header file using a different path so we can evaluate if it is recored as the same file internally.
#include "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test16/Simple.h"

Simple::Simple(){
  v1 = 15;
}

int main(int argc, char* argv[]) {
	
}
