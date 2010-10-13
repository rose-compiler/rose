#include "testCodeGeneration/testCodeBuilder.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace SageInterface;

#define foreach BOOST_FOREACH

bool isStateClassDeclaration(SgClassDeclaration* class_decl)
{
	ROSE_ASSERT(class_decl);
	string class_name = class_decl->get_name().str();
	return class_name == "State";
}


void assembleTestCode(SgProject* project)
{
	cout << "Test code loaded!\n";

	ROSE_ASSERT(project->get_fileList().size() == 1);
	SgSourceFile* source_file = isSgSourceFile((*project)[0]);
	source_file->set_unparse_output_filename(source_file->getFileName());

	SgGlobal* global_scope = source_file->get_globalScope();
	ROSE_ASSERT(global_scope);

	// Find the state class out.
	SgClassDeclaration* state_class = NULL;
	foreach (SgDeclarationStatement* decl, global_scope->get_declarations())
	{
		SgClassDeclaration* class_decl = isSgClassDeclaration(decl);
		if (class_decl  && isStateClassDeclaration(class_decl))
		{
			state_class = class_decl;
			break;
		}
	}

	if (state_class)
	{
		cout << "Start to assemble the test code\n";
		TestCodeAssembler assembler(state_class);
		assembler.assemble();
		cout << "Done!\n";
	}
}


int main(int argn, char** argv)
{
	vector<string> args(argv, argv + argn);

	// First, build a test code file.
	string filename = "test.C";
	
	// Then we create a new file which will be our output.
	ofstream ofs(filename.c_str());
	//ofs << "#define __attribute__(x) ;\n";
	//ofs << "#include <stdlib.h>\n";
	//ofs << "int foo() __attribute__((visibility(\"default\")));\n";
	ofs.close();

	args.push_back(filename);
	SgProject* project = frontend(args);

	{
		BasicExpressionTest test(project, false);
		test.build();
	}


	// Reverse event functions inside.

	// Add initialization, comparison and main functions to this file.
	assembleTestCode(project);

	AstTests::runAllTests(project);
	return backend(project);
}

