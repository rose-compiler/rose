#ifndef LINEARIZE_AST_H
#define LINEARIZE_AST_H

// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

//#include "rose.h"
#include <stack>

class LinInheritedAttribute
   {
     public:
          int loopNestDepth;

          LinInheritedAttribute ();
          LinInheritedAttribute ( const LinInheritedAttribute & X );
   };

class LinSynthesizedAttribute
   {
     public:
         LinSynthesizedAttribute();
   };


class LinearizeAST : public SgTopDownBottomUpProcessing<LinInheritedAttribute,LinSynthesizedAttribute>
   {
     private:
	  //The idea is to create an inorder ordering of the nodes using a SgTopDownBottomUp
	  //traversal. The idea is that every node can be viewed as a leaf node if you diregard
	  //it's subtree. Using 4 nodes we can describe any such operation
	  //                                 A
	  //                                 |
	  //                                 B
	  //                                /|
	  //                               C D
	  // Here we will first pop C and B. then D and A. 
	  // In another example
	  //                                 A
	  //                                 |
	  //                                 B
	  //                                 |
	  //                                 C
	  // we will first pop C and B, then A.
	  //
	  // PS! This operation will only be applied on expression. For all other constructs the
	  // ordering of the preorder traversal is used.

     public:
std::vector<SgNode*> inorder; 
          std::vector<SgNode*> inorder_stack; 


       // Functions required
          LinInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             LinInheritedAttribute inheritedAttribute );

          LinSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             LinInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );

	  std::vector<SgNode*>
	  get_linearization();
	  void printVector(std::vector<SgNode*>& vec);
   };


std::vector<SgNode*>
linearize_subtree( SgNode* node );

// extern bool VERBOSE_MESSAGES_OF_WAVE;


#endif
