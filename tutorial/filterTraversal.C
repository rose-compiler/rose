/**********************************************************
 * Example demonstates how to use the filtering
 * traversal in ROSE. The filtering traversal is a mechanism
 * for selectively not traverse subtrees or nodes in the AST.
 *
 * In this specific example it is demonstrated how you can
 * selectively traverse only the constant folded expression or
 * the original expression tree for an expression that has been
 * constant folded. The default behavior of the traversal mechaism
 * is to traverse both.
 *************************************************************/
#include "rose.h"
#include <time.h>
//Include the header file for the filtering traversal
#include <AstTraversal.h>
using namespace std;


/**********************************************************
 * This is a class implementing the methods specifying how to fitler on
 * nodes and subtrees. The AstPreOrderTraversal implements the
 * traversal itself.
 **********************************************************/
class TestTrav : public AstPreOrderTraversal
   {
     public:
       // Enum for the different ways to traverse constant folded expressions
          enum ConstantFolding
             {
               TraverseOriginalExpressionTree,
               TraverseConstantFoldedExpression,
               TraverseBoth
             };

     private:
       // Variable determining which subtree to traverse
          ConstantFolding constFoldTraverse;

     public:
          TestTrav(ConstantFolding c)
             {
               constFoldTraverse = c;
             }

       // A function called on each node in the traversal
          void preOrderVisit(SgNode* node)
             {
               std::cout << node->class_name() << " " << node << std::endl;
             }

       // Return true if a node is to be filtered out, but the subtree will be traversed.
          bool skipNode(SgNode* node)
             {  
               bool skip = false;
               SgValueExp* valExp = isSgValueExp(node);
               if( ( valExp != NULL ) && ( valExp->get_originalExpressionTree() != NULL ))
                  {
                    if ( constFoldTraverse == TraverseOriginalExpressionTree )
                         skip=true;
                  }
               return skip; 
             }

       // Returns true if the subtree of the node should not be traversed, but the node itself will be traversed.
          bool skipSubTreeOfNode(SgNode* node)
             { 
               bool skip =false;
               SgValueExp* valExp = isSgValueExp(node);
               if( ( valExp != NULL ) && ( valExp->get_originalExpressionTree() != NULL ))
                  {
                    if( constFoldTraverse == TraverseConstantFoldedExpression )
                         skip=true;
                  }
               return skip; 
             }
   };

int main( int argc, char * argv[] ) 
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);

  // Variable determining which part of a constant folded expression should be traversed
     TestTrav::ConstantFolding c;
     c = TestTrav::TraverseOriginalExpressionTree;

     TestTrav* testTrav = new TestTrav(c);

  // Find the subtree of the AST that we want to traverse
     list<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgAssignInitializer);

  // Initiate the traversal
     testTrav->traverse(*functionDeclarationList.begin()); 

     return backend(project);
   }

