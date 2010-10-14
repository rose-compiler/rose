#include <list>
using namespace std;

#include "rose.h"
#define SgDefaultFile Sg_File_Info::generateDefaultFileInfoForTransformationNode()

void convertCommaOp(SgProject* project);
SgProject* project;

int main(int argc, char * argv[]) 
{	
	// Build the AST used by ROSE
	project = frontend(argc,argv);

	convertCommaOp(project);

	// Run internal consistancy tests on AST
	AstTests::runAllTests(project);	


	return backend(project);	
}

void convertCommaOp(SgProject* project)
{	
	// look through all the pragma declarations inside of function bodies
	Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
	{
		SgFunctionDefinition* func = isSgFunctionDefinition(*i);
		ROSE_ASSERT(func);
		
		SgBasicBlock* funcBody = func->get_body();
		
		Rose_STL_Container<SgNode*> pragmas = NodeQuery::querySubTree(funcBody, V_SgValueExp );
		for (Rose_STL_Container<SgNode*>::const_iterator it = pragmas.begin(); it != pragmas.end(); ++it)
		{
			SgValueExp* valExp = isSgValueExp(*it); ROSE_ASSERT(valExp);
			
			printf("valExp=<%s | %s>\n", valExp->class_name().c_str(), valExp->unparseToString().c_str());
			printf("valExp->get_parent()=<%s | %s>\n", valExp->get_parent()->class_name().c_str(), valExp->get_parent()->unparseToString().c_str());
			printf("valExp->get_parent()->get_parent()=<%s | %s>\n", valExp->get_parent()->get_parent()->class_name().c_str(), valExp->get_parent()->get_parent()->unparseToString().c_str());
			
			if(isSgAssignInitializer(valExp->get_parent()))
			{
				SgAssignInitializer* parent = isSgAssignInitializer(valExp->get_parent());
				SgInitializedName* initName = isSgInitializedName(parent->get_parent()); ROSE_ASSERT(initName);
				
				SgIntVal* num0 = new SgIntVal(SgDefaultFile, 0);
				SgTypeInt* type = new SgTypeInt();
				SgCommaOpExp* wrapper = new SgCommaOpExp(SgDefaultFile, valExp, num0, type);
				valExp->set_parent(wrapper);
				num0->set_parent(wrapper);
				
				type = new SgTypeInt();
				SgAssignInitializer* newInit = new SgAssignInitializer(SgDefaultFile, wrapper, type);
				wrapper->set_parent(newInit);
				
				initName->set_initializer(newInit);
				newInit->set_parent(initName);
			}
		}
	}
}
