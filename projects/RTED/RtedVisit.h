#ifndef RTEDVISITVAR_H
#define RTEDVISITVAR_H

#include "RtedTransformation.h"

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class InheritedAttribute
   {
     public:
      // Depth in AST
         bool global;
         bool function;
         bool isAssignInitializer;
         bool isArrowExp;
         bool isAddressOfOp;
         bool isLValue;

       // Specific constructors are required
	 InheritedAttribute (bool g, bool f, bool a, bool ae, bool ao, bool l) : global(g),
	                                               function(f),
	                                               isAssignInitializer(a),
	                                               isArrowExp(ae),
	                                               isAddressOfOp(ao),
	                                               isLValue(l) {};
	 InheritedAttribute ( const InheritedAttribute & X ) : global(X.global),
	                                                       function(X.function),
	                                                       isAssignInitializer(X.isAssignInitializer),
	                                                       isArrowExp(X.isArrowExp),
	                                                       isAddressOfOp(X.isAddressOfOp),
	                                                       isLValue(X.isLValue) {};
   };

typedef bool SynthesizedAttribute;



class VariableTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     RtedTransformation* transf;
     public:

   VariableTraversal(RtedTransformation* t) : transf(t) {};
     ~VariableTraversal() {};

     // Functions required
     InheritedAttribute evaluateInheritedAttribute (
						    SgNode* astNode, 
						    InheritedAttribute inheritedAttribute );
     
     SynthesizedAttribute evaluateSynthesizedAttribute (
							SgNode* astNode,
							InheritedAttribute inheritedAttribute,
							SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

#endif
