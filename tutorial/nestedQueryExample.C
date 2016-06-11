// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

using namespace std;


 // Function querySolverAccessFunctions() 
 // find access functions (function name starts with "get_" or "set_")
NodeQuerySynthesizedAttributeType
querySolverAccessFunctions (SgNode * astNode)
   {
     ROSE_ASSERT (astNode != 0);
     NodeQuerySynthesizedAttributeType returnNodeList;

     SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(astNode);

     if (funcDecl != NULL)
        {
          string functionName = funcDecl->get_name().str();
          if ( (functionName.length() >= 4) && ((functionName.substr(0,4) == "get_") || (functionName.substr(0,4) == "set_")) )
               returnNodeList.push_back (astNode);
        }

     return returnNodeList;
   }

// Function printFunctionDeclarationList will print all function names in the list
void printFunctionDeclarationList(Rose_STL_Container<SgNode*> functionDeclarationList)
   {
     int counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
        {
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(functionDeclaration != NULL);

       // output the function number and the name of the function
          printf ("function name #%d is %s at line %d \n",
               counter++,functionDeclaration->get_name().str(),
               functionDeclaration->get_file_info()->get_line());
        }
   }

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

  // Build a list of functions within the AST and find all access functions 
  // (function name starts with "get_" or "set_")

  // Build list using a query of the whole AST
     Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);

  // Build list using nested Queries (operating on return result of previous query)
     Rose_STL_Container<SgNode*> accessFunctionsList;
     accessFunctionsList = NodeQuery::queryNodeList (functionDeclarationList,&querySolverAccessFunctions);
     printFunctionDeclarationList(accessFunctionsList);

  // Alternative form of same query building the list using a query of the whole AST
     accessFunctionsList = NodeQuery::querySubTree (project,&querySolverAccessFunctions);
     printFunctionDeclarationList(accessFunctionsList);

  // Another way to query for collections of IR nodes
     VariantVector vv1 = V_SgClassDefinition;
     std::cout << "Number of class definitions in the memory pool is: " << NodeQuery::queryMemoryPool(vv1).size() << std::endl;

  // Another way to query for collections of multiple IR nodes.
  // VariantVector(V_SgType) is internally expanded to all IR nodes derived from SgType.
     VariantVector vv2 = VariantVector(V_SgClassDefinition) + VariantVector(V_SgType);
     std::cout << "Number of class definitions AND types in the memory pool is: " << NodeQuery::queryMemoryPool(vv2).size() << std::endl;

  // Note: Show composition of AST queries

     return 0;
   }

