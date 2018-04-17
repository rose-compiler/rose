#include "rose.h"

using namespace std;
using namespace SageInterface;


#ifdef FUNC_DECL
#define DELETED_AST_NODE(n) isSgFunctionDeclaration(n)
#define VNode V_SgFunctionDeclaration
#define PRINTEDSTRING "SgFunctionDeclaration\n"
#else 
#	ifdef FUNC_DEF 
#	define DELETED_AST_NODE(n) isSgFunctionDefinition(n)
#	define VNode V_SgFunctionDefinition
#	define PRINTEDSTRING "SgFunctionDefinition\n"
#	else 
#		ifdef VAR_DECL
#		define DELETED_AST_NODE(n) isSgVariableDeclaration(n)
#       	define VNode V_SgVariableDeclaration
#		define PRINTEDSTRING "SgVariableDeclaration\n"
#		else
#			ifdef VAR_DEF
#			define DELETED_AST_NODE(n) isSgVariableDefinition(n)
#          	        define VNode V_SgVariableDefinition
#			define PRINTEDSTRING "SgVariableDefinition\n"
#			else
#				ifdef GLOBAL
#				define DELETED_AST_NODE(n) isSgGlobal(n)
#                       	define VNode V_SgGlobal
#				define PRINTEDSTRING "SgGlobal\n"
#				endif	
#			endif
#		endif
#	endif
#endif



int main ( int argc, char* argv[] )
   {
     if (SgProject::get_verbose() > 0)
          printf ("In visitorTraversal.C: main() \n");

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);
     AstTests::runAllTests(project);

     Rose_STL_Container<SgNode*> ItemList = NodeQuery::querySubTree (project,VNode);
     for (Rose_STL_Container<SgNode*>::iterator i = ItemList.begin(); i != ItemList.end(); i++){
          SgNode* node = DELETED_AST_NODE(*i);
          ROSE_ASSERT(node != NULL);
	  //if(i==ItemList.begin())printf(PRINTEDSTRING);
          SgNode * copy = deepCopy(node);
          deepDelete(copy);
     }


     SgNode * copyproject = deepCopy(project);
     deepDelete(copyproject);

     const int MAX_NUMBER_OF_IR_NODES = 2000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES);
     AstTests::runAllTests(project);
   }

