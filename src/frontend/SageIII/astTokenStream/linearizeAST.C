// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"
#include "linearizeAST.h"
LinInheritedAttribute::LinInheritedAttribute () : loopNestDepth(0) {};
LinInheritedAttribute::LinInheritedAttribute ( const LinInheritedAttribute & X ) {};

LinSynthesizedAttribute::LinSynthesizedAttribute() {};


LinInheritedAttribute
LinearizeAST::evaluateInheritedAttribute (
     SgNode* astNode,
     LinInheritedAttribute inheritedAttribute )
   {

     if( (isSgLocatedNode(astNode)!=NULL) && ( (isSgScopeStatement(astNode)!=NULL)||(isSgValueExp(astNode)!=NULL)||(isSgVarRefExp(astNode)!=NULL)|| (isSgLocatedNode(astNode)->get_file_info()->isCompilerGenerated() == false)) ){

	  if(inorder_stack.size()>0){
	       inorder_stack.push_back(astNode);
	  }else if( isSgExprStatement(astNode) != NULL ){
	       inorder_stack.push_back(astNode);
	  }else{
	       inorder.push_back(astNode);
           if(isSgVariableDeclaration(astNode)!=NULL){
               SgInitializedNamePtrList& vars = isSgVariableDeclaration(astNode)->get_variables();
               for(SgInitializedNamePtrList::iterator it_var = vars.begin(); it_var != vars.end();
                   ++it_var){
                   inorder.push_back(*it_var);
               }
           }

	  }
     }
     return inheritedAttribute;
   }

LinSynthesizedAttribute
LinearizeAST::evaluateSynthesizedAttribute (
     SgNode* astNode,
     LinInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     LinSynthesizedAttribute returnAttribute;

     // Pop the last element in the vector
     // build by the inherited attribute as this
     // is the current node.

     if( (inorder_stack.size()>0) && ( find(inorder.begin(),inorder.end(),astNode) == inorder.end() ) ){

	  int max_size = inorder_stack.size()-1;
	  inorder.push_back( inorder_stack[max_size] );

	  //std::cout << inorder_stack[max_size]->class_name() << std::endl;
	  inorder_stack.pop_back();


       // Pop back the parent node of the current node
       // if there is such a parent node on the stack.
	  if( max_size >= 1){
	       inorder.push_back(inorder_stack[max_size-1]);
	  //     std::cout << "BLABLABLA" << inorder_stack[max_size-1]->class_name() << std::endl;

	       inorder_stack.pop_back();
	  }

     }else if( isSgScopeStatement(astNode) != NULL ){
	  //Double count Scope Statements
	  inorder.push_back(astNode);
     }

     return returnAttribute;
   }


void LinearizeAST::printVector(std::vector<SgNode*>& vec){
 
     for(std::vector<SgNode*>::iterator it_beg = vec.begin();
		     it_beg != vec.end(); ++it_beg){
	   std::cout << (*it_beg)->class_name() << std::endl;
     };	  
};

std::vector<SgNode*>
LinearizeAST::get_linearization(){

     return inorder; 
}

std::vector<SgNode*>
linearize_subtree( SgNode* node )
   {
  // Build the inherited attribute
     LinInheritedAttribute inheritedAttribute;

     LinearizeAST myTraversal;

  // Call the traversal starting at the sageProject node of the AST
     myTraversal.traverse(node,inheritedAttribute);
  // if(VERBOSE_MESSAGES_OF_WAVE==true)
  //      myTraversal.printVector(myTraversal.inorder);
     return myTraversal.get_linearization();
   }



