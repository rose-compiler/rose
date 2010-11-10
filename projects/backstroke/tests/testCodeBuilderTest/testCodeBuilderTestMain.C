#include <backstroke.h>
#include <pluggableReverser/expAndStmtHandlers.h>
#include <normalizations/expNormalization.h>
#include <testCodeGeneration/testCodeBuilder.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <numeric>
#include <boost/timer.hpp>
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


void assembleTestCode(SgProject* project, const vector<ProcessedEvent>& results)
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
	//event_processor.addExpressionHandler(new AkgulStyleExpressionHandler);

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

void test(TestCodeBuilder* builder, const vector<string>& args)
{
	cout << "1. Generating test code now.\n";

	string filename = builder->getFileName();
	bool need_build = true;//!filesystem::exists(filename);

	SgProject* project = NULL;
	if (need_build)
	{
		builder->buildTestCode();
	}

	string processed_filename = "processed_" + builder->getFileName();
	bool need_reverse = true;//!filesystem::exists(processed_filename);
	if (!need_reverse)
		return;

	vector<string> new_args(args);
	new_args.push_back("-c");
	new_args.push_back(filename);
	project = frontend(new_args);


	cout << "2. Reversing events now.\n";

	// Reverse event functions inside.
	EventProcessor event_processor;
	addHandlers(event_processor);
	vector<ProcessedEvent> results =
			Backstroke::reverseEvents(
				&event_processor,
				isEvent2,//bind(isEvent, _1, events),
				project);

	cout << "Events reversed.\n";


	// Add initialization, comparison and main functions to this file.
	assembleTestCode(project, results);

	AstTests::runAllTests(project);

	SgSourceFile* source_file = isSgSourceFile((*project)[0]);
	source_file->set_unparse_output_filename(processed_filename);
	backend(project);
}

int main(int argc, char* argv[])
{
	timer t;

	vector<string> args(argv, argv + argc);

	//BasicExpressionTest complex_exp_test("BasicExpressionTest2");
	//ComplexExpressionTest complex_exp_test("ComplexExpressionTest");
	test(new BasicExpressionTest("BasicExpressionTest"), args);
	//test(&complex_exp_test, args);

	cout << "Time used: " << t.elapsed() << endl;
}
