#include <backstroke.h>
#include <pluggableReverser/expAndStmtHandlers.h>
#include <normalizations/expNormalization.h>
#include <testCodeGeneration/testCodeBuilder.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

#include <numeric>
//#include <limits.h>

using namespace std;
using namespace boost;
//using namespace boost::lambda;
using namespace SageInterface;
using namespace SageBuilder;

#define foreach BOOST_FOREACH

bool isStateClassDeclaration(SgClassDeclaration* class_decl)
{
	ROSE_ASSERT(class_decl);
	string class_name = class_decl->get_name().str();
	return class_name == "State";
}


void assembleTestCode(SgProject* project, const map<SgFunctionDeclaration*, FuncDeclPairs>& results)
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
		TestCodeAssembler assembler(state_class, results);
		assembler.assemble();
		cout << "Done!\n";
	}
}

void addHandlers(EventProcessor& event_processor)
{
	// Add all expression handlers to the expression pool.
	//event_processor.addExpressionHandler(new NullExpressionHandler);
	event_processor.addExpressionHandler(new IdentityExpressionHandler);
	event_processor.addExpressionHandler(new StoreAndRestoreExpressionHandler);
	event_processor.addExpressionHandler(new AkgulStyleExpressionHandler);

	// Add all statement handlers to the statement pool.
	event_processor.addStatementHandler(new CombinatorialExprStatementHandler);
	event_processor.addStatementHandler(new VariableDeclarationHandler);
	event_processor.addStatementHandler(new CombinatorialBasicBlockHandler);
	event_processor.addStatementHandler(new IfStatementHandler);
	event_processor.addStatementHandler(new WhileStatementHandler);
	event_processor.addStatementHandler(new ReturnStatementHandler);
	event_processor.addStatementHandler(new StateSavingStatementHandler);
	event_processor.addStatementHandler(new NullStatementHandler);

	//Variable value extraction handlers
	event_processor.addVariableValueRestorer(new RedefineValueRestorer);
	event_processor.addVariableValueRestorer(new ExtractFromUseValueRestorer);
}

bool isEvent(SgFunctionDeclaration* decl, const vector<SgFunctionDeclaration*>& events)
{
	return std::find(events.begin(), events.end(), decl) != events.end();
}

bool isEvent2(SgFunctionDeclaration* func)
{
	string func_name = func->get_name();
	if (starts_with(func_name, "event") &&
			!ends_with(func_name, "reverse") &&
			!ends_with(func_name, "forward"))
		return true;
	return false;
}


int main(int argc, char* argv[])
{
	vector<string> args(argv, argv + argc);

	// First, build a test code file.
	string filename = "test.C";
	
	// Then we create a new file which will be our output.
	ofstream ofs(filename.c_str());
	ofs.close();

	args.push_back(filename);
	SgProject* project = frontend(args);

	cout << "1. Generating test code now.\n";

	vector<SgFunctionDeclaration*> events;
	ComplexExpressionTest test(project, false);
	//BasicExpressionTest test(project, false);
	test.build();
	events = test.getAllEvents();

	// Before we go forward to the next step, check the project here.
	AstTests::runAllTests(project);

	// Unparse the test code to help to find errors.
	backend(project);

	cout << "2. Reversing events now.\n";
	
	// Reverse event functions inside.
	EventProcessor event_processor;
	addHandlers(event_processor);
	map<SgFunctionDeclaration*, FuncDeclPairs> results =
			Backstroke::reverseEvents(
				&event_processor,
				isEvent2,//bind(isEvent, _1, events),
				project);

	cout << "Events reversed.\n";


	// Add initialization, comparison and main functions to this file.
	assembleTestCode(project, results);

	AstTests::runAllTests(project);
	return backend(project);
}
