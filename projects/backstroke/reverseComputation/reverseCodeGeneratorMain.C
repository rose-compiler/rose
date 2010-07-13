#include "eventReverser.h"
#include "facilityBuilder.h"
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

class EventReverserTraversal : public AstSimpleProcessing
{
public:
	EventReverserTraversal(): events_num(0), model_type(NULL)
	{
	}
	virtual void visit(SgNode* n);

	int events_num;
	SgClassType* model_type;

	/** Map from a function declaration to the corresponding forward and reverse methods. */
	map<SgFunctionDeclaration*, FuncDeclPair> generatedFunctions;

	vector<SgVariableDeclaration*> generatedVariableDeclarations;
	vector<SgAssignOp*> generatedVariableInitializations;

	vector<string> event_names;
private:
	
};


void EventReverserTraversal::visit(SgNode* n)
{
	if (SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(n))
	{
		string func_name = func_decl->get_name();
		if (!starts_with(func_name, "event") ||
				ends_with(func_name, "reverse") ||
				ends_with(func_name, "forward"))
			return;

		event_names.push_back(func_name);

		EventReverser reverser(func_decl, NULL);
		map<SgFunctionDeclaration*, FuncDeclPair> func_pairs = reverser.outputFunctions();
		generatedFunctions.insert(func_pairs.begin(), func_pairs.end());

		// Collect all variables needed to be declared
		vector<SgVariableDeclaration*> decls = reverser.getVarDeclarations();
		vector<SgAssignOp*> inits = reverser.getVarInitializers();

		generatedVariableDeclarations.insert(generatedVariableDeclarations.end(), decls.begin(), decls.end());
		generatedVariableInitializations.insert(generatedVariableInitializations.end(), inits.begin(), inits.end());

		// increase the number of events
		++events_num;
	}

	// Get the model structure type which will be used in other functions, like initialization.
	if (SgClassDeclaration * model_decl = isSgClassDeclaration(n))
	{
		if (model_decl->get_name() == "model")
			model_type = model_decl->get_type();
	}
}


int fixVariableReferences2(SgNode* root)
{
	ROSE_ASSERT(root);
	int counter = 0;
	Rose_STL_Container<SgNode*> nodeList;

	SgVarRefExp* varRef = NULL;
	Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(root, V_SgVarRefExp);
	for (Rose_STL_Container<SgNode*>::iterator i = reflist.begin(); i != reflist.end(); i++)
	{
		//cout << get_name(isSgVarRefExp(*i)) << endl;
		varRef = isSgVarRefExp(*i);
		ROSE_ASSERT(varRef->get_symbol());
		SgInitializedName* initname = varRef->get_symbol()->get_declaration();
		//ROSE_ASSERT(initname);

		if (initname->get_type() == SgTypeUnknown::createType())
			//    if ((initname->get_scope()==NULL) && (initname->get_type()==SgTypeUnknown::createType()))
		{
			SgName varName = initname->get_name();
			SgSymbol* realSymbol = NULL;
			//cout << varName << endl;

			// CH (5/7/2010): Before searching SgVarRefExp objects, we should first deal with class/structure
			// members. Or else, it is possible that we assign the wrong symbol to those members if there is another
			// variable with the same name in parent scopes. Those members include normal member referenced using . or ->
			// operators, and static members using :: operators.
			//
			if (SgArrowExp * arrowExp = isSgArrowExp(varRef->get_parent()))
			{
				if (varRef == arrowExp->get_rhs_operand())
				{
					// make sure the lhs operand has been fixed
					counter += fixVariableReferences2(arrowExp->get_lhs_operand());

					SgPointerType* ptrType = isSgPointerType(arrowExp->get_lhs_operand()->get_type());
					ROSE_ASSERT(ptrType);
					SgClassType* clsType = isSgClassType(ptrType->get_base_type());
					ROSE_ASSERT(clsType);
					SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
					decl = isSgClassDeclaration(decl->get_definingDeclaration());
					ROSE_ASSERT(decl);
					realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
				}
				else
					realSymbol = lookupSymbolInParentScopes(varName, getScope(varRef));
			}
			else if (SgDotExp * dotExp = isSgDotExp(varRef->get_parent()))
			{
				if (varRef == dotExp->get_rhs_operand())
				{
					// make sure the lhs operand has been fixed
					counter += fixVariableReferences2(dotExp->get_lhs_operand());

					SgClassType* clsType = isSgClassType(dotExp->get_lhs_operand()->get_type());
					ROSE_ASSERT(clsType);
					SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
					decl = isSgClassDeclaration(decl->get_definingDeclaration());
					ROSE_ASSERT(decl);
					realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
				}
				else
					realSymbol = lookupSymbolInParentScopes(varName, getScope(varRef));
			}
			else
				realSymbol = lookupSymbolInParentScopes(varName, getScope(varRef));

			// should find a real symbol at this final fixing stage!
			// This function can be called any time, not just final fixing stage
			if (realSymbol == NULL)
			{
				cerr << "Error: cannot find a symbol for " << varName.getString() << endl;
				ROSE_ASSERT(realSymbol);
			}
			else
			{
				// release placeholder initname and symbol
				ROSE_ASSERT(realSymbol != (varRef->get_symbol()));

				bool flag = false;

				SgSymbol* symbol_to_delete = varRef->get_symbol();
				varRef->set_symbol(isSgVariableSymbol(realSymbol));
				counter++;

				if (nodeList.empty())
				{
					VariantVector vv(V_SgVarRefExp);
					nodeList = NodeQuery::queryMemoryPool(vv);
				}


				foreach(SgNode* node, nodeList)
				{
					if (SgVarRefExp * var = isSgVarRefExp(node))
					{
						if (var->get_symbol() == symbol_to_delete)
						{
							flag = true;
							break;
						}
					}
				}
				if (!flag)
				{
					//cout <<initname->get_name().str() << endl;
					delete initname; // TODO deleteTree(), release File_Info nodes etc.
					delete symbol_to_delete;
				}
			}
		}
	} // end for
	return counter;
}


int main(int argc, char * argv[])
{
	vector<string> args(argv, argv + argc);
	bool klee = CommandlineProcessing::isOption(args, "-backstroke:", "klee", true);
	SgProject* project = frontend(args);

	EventReverserTraversal reverser;

	SgGlobal *globalScope = getFirstGlobalScope(project);
	string includes = "#include \"rctypes.h\"\n"
			"#include <stdio.h>\n"
			"#include <stdlib.h>\n"
			"#include <time.h>\n"
			"#include <assert.h>\n"
			"#include <memory.h>\n";
	if (klee)
		includes += "#include <klee.h>\n";
	addTextForUnparser(globalScope, includes, AstUnparseAttribute::e_before);

	pushScopeStack(isSgScopeStatement(globalScope));

	reverser.traverseInputFiles(project, preorder);
	cout << "Traverse complete\n";

	ROSE_ASSERT(reverser.model_type);

	//Insert all the variable declarations
	for (size_t i = 0; i < reverser.generatedVariableDeclarations.size(); ++i)
		prependStatement(reverser.generatedVariableDeclarations[i]);

	//Insert all the generated functions right after the original function
	pair<SgFunctionDeclaration*, FuncDeclPair> originalAndInstrumented;
	foreach(originalAndInstrumented, reverser.generatedFunctions)
	{
		SgFunctionDeclaration* originalFunction = originalAndInstrumented.first;
		SgFunctionDeclaration* forward = originalAndInstrumented.second.first;
		SgFunctionDeclaration* reverse = originalAndInstrumented.second.second;
		SageInterface::insertStatementAfter(originalFunction, forward);
		SageInterface::insertStatementAfter(originalFunction, reverse);
	}

	appendStatement(buildInitializationFunction(reverser.model_type));
	appendStatement(buildCompareFunction(reverser.model_type));
	appendStatement(buildMainFunction(reverser.generatedVariableInitializations, reverser.event_names, klee));


	popScopeStack();

#if 1
	cout << "Start to fix variables references\n";
	//SageInterface::fixVariableReferences(globalScope);
	fixVariableReferences2(globalScope);
	cout << "Fix finished\n";
#endif

	AstTests::runAllTests(project);
	return backend(project);
}


