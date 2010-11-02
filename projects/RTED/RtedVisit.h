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
         bool isLValue;
         bool isReferenceType;
         bool isInitializedName;
         bool isBinaryOp;
         bool isDotExp;
         bool isPointerDerefExp;


 //  InheritedAttributeBools* bools;
       // Specific constructors are required

	 InheritedAttribute (bool f, bool a, bool ae, bool ao, bool l, bool r, bool i, bool bo, bool dot, bool pd) :
	                                               
	                                               function(f),
	                                               isAssignInitializer(a),
	                                               isArrowExp(ae),
	                                               isAddressOfOp(ao),
	                                               isLValue(l),
	                                               isReferenceType(r),
	                                               isInitializedName(i),
	                                               isBinaryOp(bo),
	                                               isDotExp(dot),
	                                               isPointerDerefExp(pd) {};
	 InheritedAttribute ( const InheritedAttribute & X ) : 
	                                                       function(X.function),
	                                                       isAssignInitializer(X.isAssignInitializer),
	                                                       isArrowExp(X.isArrowExp),
	                                                       isAddressOfOp(X.isAddressOfOp),
	                                                       isLValue(X.isLValue),
	                                                       isReferenceType(X.isReferenceType),
	                                                       isInitializedName(X.isInitializedName),
	                                                       isBinaryOp(X.isBinaryOp),
	                                                       isDotExp(X.isDotExp),
	                                                       isPointerDerefExp(X.isPointerDerefExp) {};

 //  InheritedAttribute (InheritedAttributeBools* b) : bools(b){};
 //  InheritedAttribute ( const InheritedAttribute & X ) : bools(X.bools){};
   };

typedef bool SynthesizedAttribute;



class VariableTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {

     RtedTransformation* transf;
     std::vector<SgExpression*>* rightOfbinaryOp;
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

     bool isLValue(SgNode* node);
     bool isInterestingAssignNode(SgNode* node);
     bool isRightOfBinaryOp(SgNode* node);

     SgForStatement* isVariableInForStatement(SgNode* astNode);
     bool isInitializedNameInForStatement(SgForStatement* for_stmt,SgInitializedName* name);
   };

#endif
