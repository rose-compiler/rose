#ifndef RTEDVISITVAR_H
#define RTEDVISITVAR_H

#include "RtedTransformation.h"


// Build an inherited attribute for the tree traversal to test the rewrite mechanism

class InheritedAttribute
   {
     public:
      // Depth in AST

         bool function;
         bool isAssignInitializer;
         bool isArrowExp;
         bool isAddressOfOp;
         bool isForStatement;
         bool isBinaryOp;


 //  InheritedAttributeBools* bools;
       // Specific constructors are required

	 InheritedAttribute (bool f, bool a, bool ae, bool ao, bool i, bool bo) :
	                                               function(f),
	                                               isAssignInitializer(a),
	                                               isArrowExp(ae),
	                                               isAddressOfOp(ao),
	                                               isForStatement(i),
	                                               isBinaryOp(bo)
         {};
	 InheritedAttribute ( const InheritedAttribute & X ) : 
	                                                       function(X.function),
	                                                       isAssignInitializer(X.isAssignInitializer),
	                                                       isArrowExp(X.isArrowExp),
	                                                       isAddressOfOp(X.isAddressOfOp),
	                                                       isForStatement(X.isForStatement),
	                                                       isBinaryOp(X.isBinaryOp)
	 {};

 //  InheritedAttribute (InheritedAttributeBools* b) : bools(b){};
 //  InheritedAttribute ( const InheritedAttribute & X ) : bools(X.bools){};
   };

typedef bool SynthesizedAttribute;



class VariableTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {

     RtedTransformation* transf;
     std::vector<SgExpression*>* rightOfbinaryOp;
     std::vector<SgForStatement*>* for_stmt;
     public:

   VariableTraversal(RtedTransformation* t) ;
     ~VariableTraversal() {};

     // Functions required
     InheritedAttribute evaluateInheritedAttribute (
						    SgNode* astNode, 
						    InheritedAttribute inheritedAttribute );
     
     SynthesizedAttribute evaluateSynthesizedAttribute (
							SgNode* astNode,
							InheritedAttribute inheritedAttribute,
							SubTreeSynthesizedAttributes synthesizedAttributeList );

     bool isRightOfBinaryOp(SgNode* node);
     bool isInitializedNameInForStatement(SgForStatement* for_stmt,SgInitializedName* name);
   };

#endif
