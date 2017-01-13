#include "rose.h"

int main( int argc, char * argv[] ) 
{
	// Build the AST used by ROSE
	SgProject* project = frontend(argc,argv);

	// Run internal consistency tests on AST
	AstTests::runAllTests(project);
	// Insert your own manipulation of the AST here...

	// Generate source code from AST and call the vendor's compiler
	return backend(project);
}

