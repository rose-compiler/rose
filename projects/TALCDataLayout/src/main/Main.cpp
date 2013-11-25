/*
 * Main.cpp
 *
 *  Created on: Oct 15, 2012
 *      Author: Kamal Sharma
 */

#include "Main.hpp"

void setTrace(Rose_STL_Container<string> &cmdLineArgs)
{
	if ((CommandlineProcessing::isOption (cmdLineArgs,"--verbose","",true))
			|| (CommandlineProcessing::isOption (cmdLineArgs,"-v","",true)) )
	{
		tracePtr = &std::cout;
		verbose = true;
	}
	else
	{
		static std::ofstream null("/dev/null");
		static std::ostream nullTrace(null.rdbuf());
		nullPtr = &null;
		tracePtr = &nullTrace;
		verbose = false;
	}
}

/**
 * Creates the different Modules
 * @param cmdLineArgs
 */
void createModules(Rose_STL_Container<string> &cmdLineArgs, vector<ModuleBase *> *moduleList) {

	setTrace(cmdLineArgs);

	// Parse the Meta File
	string filename = "";
	if (CommandlineProcessing::isOptionWithParameter(cmdLineArgs, "--meta", "*",filename,
			true)) {
		trace << "Filename set" << filename << std::endl;
	}
	Parser *parser = new Parser(filename);
	meta = parser->parseFile();
	trace << " Printing Meta " << endl;
	meta->print();
	trace << " Meta Processing done." << endl;

	// Array of Struct Module Check
	AOSModuleCheck *aosModuleCheck = new AOSModuleCheck(cmdLineArgs, meta);
	moduleList->push_back(aosModuleCheck);

	// Array of Struct Module
	AOSModule *aosModule = new AOSModule(cmdLineArgs, meta);
	moduleList->push_back(aosModule);

	// Hardware Optimization Module
	HaOptModule *haoptModule = new HaOptModule(cmdLineArgs, meta);
	moduleList->push_back(haoptModule);

}

/**
 * Visits each of the Modules
 * @param sageProject
 * @param moduleList
 */
void runModules(SgProject* sageProject, vector<ModuleBase *> moduleList) {
	foreach(ModuleBase * module, moduleList)
	{
		module->visit(sageProject);
	}
}

int main(int argc, char **argv) {

	Rose_STL_Container<string> args =
	CommandlineProcessing::generateArgListFromArgcArgv(argc,argv);

	if(args.size() <= 1)
        {
                printf(" No source file specified for processing.");
                return 0;
        }

	vector<ModuleBase *> moduleList;
	createModules(args, &moduleList);

// Build the AST used by ROSE
	SgProject* sageProject = frontend(args);

// Run internal consistancy tests on AST
	AstTests::runAllTests(sageProject);

	runModules(sageProject, moduleList);

	/* debug transformations */
	generateDOT(*sageProject);
	trace << "DOTPDF done\n";

	if(!verbose)
	nullPtr->close();

	// Generate source code from AST and call the vendor's compiler
	return backend(sageProject);
}

