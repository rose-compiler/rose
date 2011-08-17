#include <backstroke.h>
#include <VariableRenaming.h>
#include "utilities/cppDefinesAndNamespaces.h"
#include "normalizations/expNormalization.h"
#include "pluggableReverser/eventProcessor.h"
#include "pluggableReverser/expAndStmtHandlers.h"

using namespace std;

struct IsEvent
{
	bool operator() (SgFunctionDeclaration* decl)
	{
		string funcName = decl->get_name();
			
		//All event handler functions are named "Handle"
		if (funcName != "Handle")
			return false;
		
		
		if (SgMemberFunctionDeclaration* memFunc = isSgMemberFunctionDeclaration(decl))
		{
			SgClassDefinition* classDef = memFunc->get_class_scope();
			SgClassDeclaration* classDecl = classDef->get_declaration();
			string className = classDecl->get_name();

			if (className == "UDPSink" || className == "InterfaceReal" || className == "L2Proto802_11" 
					|| className == "WirelessLink" || className == "InterfaceReal" || className == "Timer"
                    || className == "CBRApplication" || className == "LinkReal")
			{
				return true;
			}
		}
		
		return false;
	}
};

SgType* getPointerBaseType(SgType* type)
{
	if (isSgPointerType(type))
		return isSgPointerType(type)->get_base_type();
	else if (isSgModifierType(type))
		return getPointerBaseType(isSgModifierType(type)->get_base_type());
	else if (isSgTypedefType(type))
		return getPointerBaseType(isSgTypedefType(type)->get_base_type());
	else
		ROSE_ASSERT(false);
}

struct VariableReversalFilter : public IVariableFilter
{
	virtual bool isVariableInteresting(const VariableRenaming::VarName& var) const
	{
		SgType* type = var[0]->get_type();

		if (SageInterface::isPointerType(type))
			type = getPointerBaseType(type);

		string typeName = SageInterface::get_name(type);
		if (typeName == "Simulator"/* || typeName == "Event"*/)
			return false;
		
		//Don't save event objects
//		if (SgClassType* classType = isSgClassType(type))
//		{
//			SgClassDeclaration* typeDecl = (SgClassDeclaration*)classType->get_declaration()->get_definingDeclaration();
//			ROSE_ASSERT(typeDecl != NULL);
//			SgClassDefinition* classDef = typeDecl->get_definition();
//			ROSE_ASSERT(classDef != NULL);
//			const ClassHierarchyWrapper::ClassDefSet& superclasses = classHierarchy->getAncestorClasses(classDef);
//			
//			foreach (SgClassDefinition* superclass, superclasses)
//			{
//				if (SageInterface::get_name(superclass->get_declaration()->get_type()) == "Event")
//					return false;
//			}
//		}

		return true;
	}
};

int main(int argc, char** argv)
{
	//Add the preinclude option
	vector<string> commandArguments(argv, argv + argc);
	//commandArguments.push_back("-include");
	//commandArguments.push_back("rctypes.h");

	SgProject* project = frontend(commandArguments);
	AstTests::runAllTests(project);

	VariableReversalFilter varFilter;
	EventProcessor event_processor(&varFilter);

#ifdef REVERSE_CODE_GENERATION
	//Add the handlers in order of priority. The lower ones will be used only if higher ones do not produce results
	//Expression handlers:
	event_processor.addExpressionHandler(new IdentityExpressionHandler);
	event_processor.addExpressionHandler(new AkgulStyleExpressionHandler);
	event_processor.addExpressionHandler(new StoreAndRestoreExpressionHandler);

	//Statement handler
	event_processor.addStatementHandler(new ReturnStatementHandler);
	event_processor.addStatementHandler(new VariableDeclarationHandler);
	event_processor.addStatementHandler(new StraightlineStatementHandler);
	event_processor.addStatementHandler(new NullStatementHandler);

	//Variable value extraction handlers
	event_processor.addVariableValueRestorer(new RedefineValueRestorer);
	event_processor.addVariableValueRestorer(new ExtractFromUseValueRestorer);
#else
	event_processor.addStatementHandler(new StateSavingStatementHandler);
#endif

	Backstroke::reverseEvents(&event_processor, IsEvent(), project);

	AstTests::runAllTests(project);
	return backend(project);
}
