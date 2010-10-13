#ifndef RTEDVISITVAR_H
#define RTEDVISITVAR_H

#include "RtedTransformation.h"


// Build an inherited attribute for the tree traversal to test the rewrite mechanism
/*
class InheritedAttributeBools {
public:
   bool global;
   bool function;
   bool isAssignInitializer;
   bool isArrowExp;
   bool isAddressOfOp;
   bool isLValue;
   bool isReferenceType;
   bool isInitializedName;

   InheritedAttributeBools() {

      global=false;
       function=false;
       isAssignInitializer=false;
       isArrowExp=false;
       isAddressOfOp=false;
       isLValue=false;
       isReferenceType=false;
       isInitializedName=false;
   }
   InheritedAttributeBools(bool g, bool f, bool a, bool ae, bool ao, bool l, bool r, bool i) : global(g),
                                                  function(f),
                                                  isAssignInitializer(a),
                                                  isArrowExp(ae),
                                                  isAddressOfOp(ao),
                                                  isLValue(l),
                                                  isReferenceType(r),
                                                  isInitializedName(i) {};
};
*/
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
         bool isReferenceType;
         bool isInitializedName;

 //  InheritedAttributeBools* bools;
       // Specific constructors are required

	 InheritedAttribute (bool g, bool f, bool a, bool ae, bool ao, bool l, bool r, bool i) : global(g),
	                                               function(f),
	                                               isAssignInitializer(a),
	                                               isArrowExp(ae),
	                                               isAddressOfOp(ao),
	                                               isLValue(l),
	                                               isReferenceType(r),
	                                               isInitializedName(i) {};
	 InheritedAttribute ( const InheritedAttribute & X ) : global(X.global),
	                                                       function(X.function),
	                                                       isAssignInitializer(X.isAssignInitializer),
	                                                       isArrowExp(X.isArrowExp),
	                                                       isAddressOfOp(X.isAddressOfOp),
	                                                       isLValue(X.isLValue),
	                                                       isReferenceType(X.isReferenceType),
	                                                       isInitializedName(X.isInitializedName){};

 //  InheritedAttribute (InheritedAttributeBools* b) : bools(b){};
 //  InheritedAttribute ( const InheritedAttribute & X ) : bools(X.bools){};
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

     bool isLValue(SgNode* node);
   };

#endif
