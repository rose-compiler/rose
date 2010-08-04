// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;

#if 1
void
printNodeList ( const list<SgNode*> & localList )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector
     int counter = 0;
     printf ("Output node list: \n");
     for (list<SgNode*>::const_iterator i = localList.begin(); i != localList.end(); i++)
        {
          printf ("   list element #%d = %s \n",counter,(*i)->sage_class_name());
          counter++;
        }
   }

void
printNodeList ( const list<string> & localList )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector
     int counter = 0;
     printf ("Output node list: \n");
     for (list<string>::const_iterator i = localList.begin(); i != localList.end(); i++)
        {
          printf ("   list element #%d = %s \n",counter,(*i).c_str());
          counter++;
        }
   }
#endif



int
main( int argc, char * argv[] ) 
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
  // Run internal consistancy tests on AST
     AstTests::runAllTests(project);

  // DQ (9/12/2009): At least with GNU 4.3 and 4.4 the validValue should be 95!
  // DQ (12/5/2007): This value changed as a result of more builtin functions being added.
  // DQ (1/19/2007): I think this value changed as a result of a bug fix in ROSE (variant handling fix in sage_gen_be.C)
  // const int validValue = 55;
  // const unsigned int validValue = 89;
  // const unsigned int validValue = 91;
  // const unsigned int validValue = 95;
     unsigned int validValue = 0;
#if __GNUC__ == 3
     validValue = 92;
#endif
#if __GNUC__ == 4 && __GNUC_MINOR__ <= 2
     validValue = 92;
#endif
#if __GNUC__ == 4 && __GNUC_MINOR__ > 2
     validValue = 94;
#endif

     NodeQuerySynthesizedAttributeType returnList = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
     if (returnList.size() != validValue)
        {
          printf ("Number of SgFunctionDeclaration IR nodes = %zu \n",returnList.size());
        }
     ROSE_ASSERT(returnList.size() == validValue);
     NodeQuerySynthesizedAttributeType returnListQueryList = NodeQuery::queryNodeList(returnList, V_SgFunctionDeclaration);
     if (returnListQueryList.size() != validValue)
        {
          printf ("Number of SgFunctionDeclaration IR nodes = %zu \n",returnListQueryList.size());
        }
     ROSE_ASSERT(returnListQueryList.size() == validValue);
     NameQuerySynthesizedAttributeType returnListNames = NameQuery::querySubTree(project, NameQuery::FunctionDeclarationNames);
  // printNodeList(returnListNames);
     if (returnListNames.size() != validValue)
        {
          printf ("Number of FunctionDeclarationNames IR nodes = %zu \n",returnListNames.size());
        }
     ROSE_ASSERT(returnListNames.size() == validValue);
     NameQuerySynthesizedAttributeType returnListNamesNodeList = NameQuery::queryNodeList(returnList,NameQuery::FunctionDeclarationNames);
     if (returnListNamesNodeList.size() != validValue)
        {
          printf ("Number of FunctionDeclarationNames IR nodes = %zu \n",returnListNamesNodeList.size());
        }
     ROSE_ASSERT(returnListNamesNodeList.size() == validValue);
     NumberQuerySynthesizedAttributeType returnListNumber = NumberQuery::querySubTree(project,NumberQuery::NumberOfArgsInConstructor);
     if (returnListNumber.size() != 1)
        {
          printf ("Number of NumberOfArgsInConstructor IR nodes = %zu \n",returnListNumber.size());
        }
     ROSE_ASSERT(returnListNumber.size() == 1);
     
#if 0
  // Insert your own manipulation of the AST here...
     printf ("\n\n");
     printf ("************************** \n");
     printf ("Generate list of types ... \n");
     list<SgNode*> nodeList = NodeQuery::querySubTree (project,V_SgType);
     printNodeList(nodeList);
     printf ("*** Sort list *** \n");
     nodeList.sort();
     nodeList.unique();
     printNodeList(nodeList);
     printf ("DONE: Generate list of types ... \n");
     printf ("************************** \n\n\n");

  // DQ (7/20/2004): temporary call to help debug traversal on all regression tests
  // NodeQuery::generateListOfTypes (sageProject);
#endif

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }

