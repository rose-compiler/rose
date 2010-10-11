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


void assembleTestCode(const string& file_name)
{
	vector<string> args(1, "");
	args.push_back(file_name);
	SgProject* project = frontend(args);

	cout << "Test code loaded!\n";

	SgGlobal* global_scope = getGlobalScope(project);

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

	AstTests::runAllTests(project);
	backend(project);
}


int main(int argn, char** argv)
{
	vector<string> args(argv, argv + argn);

	// First, build a test code file.
	string filename = "test.C";
	// Then we create a new empty file which will be our output.
	FILE* file = ::fopen(filename.c_str(), "w");
	::fclose(file);

	args.push_back(filename);
	SgProject* project = frontend(args);

	// Since we have only one file as the input, the first file is what we want.
	SgSourceFile* source_file = isSgSourceFile((*project)[0]);
	ROSE_ASSERT(source_file);
	source_file->set_unparse_output_filename(filename);

	BasicExpressionTest test(project, false);
	test.build();


	AstTests::runAllTests(project);
	backend(project);

	// Reverse event functions inside.


	// Add initialization, comparison and main functions to this file.
	//assembleTestCode(filename);

	return 0;
}

