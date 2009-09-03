// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;

#if 1
	void
printNodeList ( const NodeQuerySynthesizedAttributeType & localList )
   {
  // Supporting function for querySolverGrammarElementFromVariantVector
     int counter = 0;
     printf ("Output node list: \n");
     for (NodeQuerySynthesizedAttributeType::const_iterator i = localList.begin(); i != localList.end(); i++)
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


/*****************************************************************************
  * The functional
  *    class FunctionalTest1 
  * will stress test the querySubTree(..., AstQueryNamespace::ChildrenOnly) for
  * every pre-implemented function of one argument against the queryNodeList(...).
  * This is also indirectly a test of the 
  *    NodeQuery::querySubTree<typename NodeFunctional>(..)
  * since the querySubTree we are using builds on that interface.
  ****************************************************************************/
class FunctionalTest1 :  public std::unary_function<SgNode*, NodeQuerySynthesizedAttributeType > 
   {
     public:
	  result_type operator()(SgNode* node ) 
	     { 
	       result_type returnType;
	       returnType.push_back(node);

	       NodeQuerySynthesizedAttributeType listToNodeQuery;
	    //visit only the nodes which is pointed to by this class
	       typedef std::vector< SgNode * > DataMemberPointerType;

	       DataMemberPointerType returnData = node->get_traversalSuccessorContainer ();
	    // A child of a node is the nodes it points to.
	       for(DataMemberPointerType::iterator i = returnData.begin(); i != returnData.end(); ++i)
		  {
		 // visit the node which is pointed to by this SgNode
		    if(*i!=NULL){
			 listToNodeQuery.push_back(*i);
		    }
		  };

	       for (NodeQuery::TypeOfQueryTypeOneParameter d = NodeQuery::VariableDeclarations; d <= NodeQuery::StructDefinitions; 
			       d = NodeQuery::TypeOfQueryTypeOneParameter(d + 1)){
		    if(node!=NULL){
			 NodeQuerySynthesizedAttributeType nodeList = NodeQuery::querySubTree(node, d,AstQueryNamespace::ChildrenOnly);

			 NodeQuerySynthesizedAttributeType listFromQueryNodeList = NodeQuery::queryNodeList(listToNodeQuery,d);

			 ROSE_ASSERT( nodeList.size() == listFromQueryNodeList.size() );
		    }
	       }

	       return returnType; 
	     }
   };




	int
main( int argc, char * argv[] ) 
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
  // Run internal consistancy tests on AST
     AstTests::runAllTests(project);
  // Test that the results from running all the predefined functions using the querySubTree and
  // queryMemoryPool gives the same number of return values.

     for (NodeQuery::TypeOfQueryTypeOneParameter d = NodeQuery::VariableDeclarations; d <= NodeQuery::StructDefinitions; 
		     d = NodeQuery::TypeOfQueryTypeOneParameter(d + 1)){
	  NodeQuerySynthesizedAttributeType returnFromQuerySubTree = NodeQuery::querySubTree(project, d);
	  NodeQuerySynthesizedAttributeType returnFromMemoryPool   = NodeQuery::queryMemoryPool(d);

  // ROSE_ASSERT( returnFromQuerySubTree.size() == returnFromMemoryPool.size()  );
     }

     FunctionalTest1 funcTest;
     NodeQuery::queryMemoryPool(funcTest);

  // AstQueryNamespace::VariantVector ir_nodes (V_SgType);
     VariantVector ir_nodes (V_SgType);
     NodeQuerySynthesizedAttributeType listOfTypes = NodeQuery::queryMemoryPool(funcTest,&ir_nodes);

     printf ("Number of types found = %zu \n",listOfTypes.size());
  // listOfTypes.sort();
  // listOfTypes.unique();
  // GB (09/26/2007): I *think* this might be the right way to do this with vectors.
     std::sort(listOfTypes.begin(), listOfTypes.end());
     NodeQuerySynthesizedAttributeType::iterator newEnd = std::unique(listOfTypes.begin(), listOfTypes.end());
  // printf ("(after reducing to unique entries) Number of types found = %ld \n",listOfTypes.size());
     printf ("(after reducing to unique entries) Number of types found = %ld \n", (long)(newEnd - listOfTypes.begin()));

     int counter = 0;
     NodeQuerySynthesizedAttributeType::iterator i = listOfTypes.begin();
     while (i != newEnd)
        {
          printf ("list element %d = %s \n",counter,(*i)->class_name().c_str());
          i++;
          counter++;
        }

  // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }

