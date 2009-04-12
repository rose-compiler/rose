// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"
#include "linearizeAST.h"


class PostSynthesizedAttribute
   {
     public:
       std::vector<SgNode*> nodes;
         PostSynthesizedAttribute()
         {
           
         };
   };


class PostAST : public SgBottomUpProcessing<PostSynthesizedAttribute>
   {
     private:


       SgNode* rootNode;
       std::vector<SgNode*> nodes;

          PostSynthesizedAttribute evaluateSynthesizedAttribute (
              SgNode* astNode,
              SubTreeSynthesizedAttributes synthesizedAttributeList )
          {
            PostSynthesizedAttribute synAttrib;

            ROSE_ASSERT(synthesizedAttributeList.size() <= 2);

            if(synthesizedAttributeList.size()>0)
            {
              synAttrib.nodes = synthesizedAttributeList[0].nodes;
              synAttrib.nodes.push_back(astNode);

              if( synthesizedAttributeList.size() == 2 )
              {
                for(int i = 0; i < synthesizedAttributeList[1].nodes.size();
                    i++ )
                  synAttrib.nodes.push_back(synthesizedAttributeList[1].nodes[i]);

              }
            }else
              synAttrib.nodes.push_back(astNode);


            if(astNode == rootNode)
              nodes = synAttrib.nodes;
             
 
            return synAttrib;
          };

     public:

          std::vector<SgNode*> traversePreorder(SgNode* node)
          {
            rootNode = node;

            traverse(node);

            return nodes;


            
          };

   };



LinearizeInheritedAttribute::LinearizeInheritedAttribute () : loopNestDepth(0), mustBeInherited(false) {};
LinearizeInheritedAttribute::LinearizeInheritedAttribute ( const LinearizeInheritedAttribute & X ) {};



LinearizeAST::LinearizeAST()
{
       isRootNode = true;
 
};

LinearizeInheritedAttribute
LinearizeAST::evaluateInheritedAttribute (
     SgNode* astNode,
     LinearizeInheritedAttribute inheritedAttribute )
   {

     if(isRootNode == true)
       inheritedAttribute.mustBeInherited = false;

//     std::cout << ( inheritedAttribute.mustBeInherited ? "true":"false" ) <<std::endl;
     if(inheritedAttribute.mustBeInherited == true)
       return inheritedAttribute;


     if( (isSgLocatedNode(astNode)!=NULL) && ((isSgExprStatement(astNode) != NULL ) || (isSgScopeStatement(astNode)!=NULL)||(isSgValueExp(astNode)!=NULL)||(isSgVarRefExp(astNode)!=NULL)|| (isSgLocatedNode(astNode)->get_file_info()->isCompilerGenerated() == false)) ){

	 if( isRootNode == true && (isSgExpression(astNode) != NULL | isSgExprStatement(astNode) != NULL )) {
            //This is needed when the root node that is given the traversal is part of an expression
            inheritedAttribute.mustBeInherited=true;
            PostAST postAST;
            std::vector<SgNode*> inSubTree = postAST.traversePreorder(astNode);
            for(int i = 0 ; i < inSubTree.size(); i++)
              nodes.push_back(inSubTree[i]);

         }else{
           nodes.push_back(astNode);
           if(isSgVariableDeclaration(astNode)!=NULL){
             SgInitializedNamePtrList& vars = isSgVariableDeclaration(astNode)->get_variables();
             for(SgInitializedNamePtrList::iterator it_var = vars.begin(); it_var != vars.end();
                 ++it_var){
               nodes.push_back(*it_var);
             }
           }

         }

     }else
           nodes.push_back(astNode);


     //Do not register Binary ops
     
     isRootNode = false;
     return inheritedAttribute;
   }




void LinearizeAST::printVector(std::vector<SgNode*>& vec){
 
     for(std::vector<SgNode*>::iterator it_beg = vec.begin();
		     it_beg != vec.end(); ++it_beg){
	   std::cout << (*it_beg)->class_name() << std::endl;
     };	  
};

std::vector<SgNode*>
LinearizeAST::get_linearization(){

     return nodes; 
}

std::vector<SgNode*>
linearize_subtree( SgNode* node )
   {
  // Build the inherited attribute
     LinearizeInheritedAttribute inheritedAttribute;
     inheritedAttribute.mustBeInherited = false;

     LinearizeAST myTraversal;

  // Call the traversal starting at the sageProject node of the AST
     myTraversal.traverse(node,inheritedAttribute);
  // if(VERBOSE_MESSAGES_OF_WAVE==true)
  //      myTraversal.printVector(myTraversal.inorder);
     return myTraversal.get_linearization();
   }



