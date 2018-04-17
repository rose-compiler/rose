// Example ROSE Translator: used for testing ROSE infrastructure
// Failure case: when the VariantVector does not count the same number of nodes of a type as an 
// equivalent simple query

#include "rose.h"

using namespace std;

class NodesInVector :  public std::binary_function<SgNode*, std::pair< VariantVector*, int*> , void* >{
    public:
	        result_type operator()(first_argument_type node, const second_argument_type accumulatedList ) const
			{

			  if( std::find(accumulatedList.first->begin(), accumulatedList.first->end(), node->variantT() ) 
				  != accumulatedList.first->end() )
				(*accumulatedList.second)++;
                          return NULL;
			};
};

class NodesInSubTree :  public std::binary_function<SgNode*,  std::pair<int*,int*>, void* >{
    public:
	        result_type operator()(first_argument_type node, const second_argument_type numberOfNodes ) const
			{

                if( isSgStatement(node) != NULL )
				  (*numberOfNodes.first)++;
                if( isSgExpression(node) != NULL )
				  (*numberOfNodes.second)++;
                return NULL;
			};
};

int
main( int argc, char * argv[] ) 
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
  // Run internal consistancy tests on AST
     AstTests::runAllTests(project);

  // DQ (1/19/2007): I think this value changed as a result of a bug fix in ROSE (variant handling fix in sage_gen_be.C)
  // const int validValue = 55;
  // const unsigned int validValue = 53;
	 //Functional to find nodes which is in the VariantVector
	 NodesInVector nodeRecognized;


	 NodesInSubTree nodesInTree;

	 int numberOfStatementsInSimple = 0;
	 int numberOfExpressionsInSimple= 0;
	 AstQueryNamespace::querySubTree(project,std::bind2nd( nodesInTree,  std::pair< int*, int*>(&numberOfStatementsInSimple,&numberOfExpressionsInSimple)));
   
	 //Check if the number of statements is the same using VariantVectors and a simple NodeQuery
	 VariantVector v1(V_SgStatement);
	 int numberOfStatements=0;
     AstQueryNamespace::querySubTree(project,std::bind2nd( nodeRecognized,  std::pair< VariantVector*, int*>(&v1,&numberOfStatements)));

	 if ( numberOfStatementsInSimple != numberOfStatements )
        {
		  std::cout << "Number of statements using VariantVector " << numberOfStatements << 
			           " and using a simple query " << numberOfStatementsInSimple << std::endl;
        }
	 ROSE_ASSERT( numberOfStatementsInSimple == numberOfStatements );

	 //Check if the number of expressions is the same using VariantVectors and a simple NodeQuery
	 VariantVector v2(V_SgExpression);
	 int numberOfExpressions=0;
     AstQueryNamespace::querySubTree(project,std::bind2nd( nodeRecognized,  std::pair< VariantVector*, int*>(&v2,&numberOfExpressions)));

	 if ( numberOfExpressionsInSimple != numberOfExpressions )
        {
           std::cout << "Number of statements using VariantVector " << numberOfExpressions << 
			           " and using a simple query " << numberOfExpressionsInSimple << std::endl;

        }
	 ROSE_ASSERT( numberOfExpressionsInSimple == numberOfExpressions );

	 // Generate source code from AST and call the vendor's compiler
     return backend(project);
   }

