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

       // Specific constructors are required
	 InheritedAttribute (bool g, bool f) : global(g), function(f) {};
	 InheritedAttribute ( const InheritedAttribute & X ) : global(X.global), function(X.function){};
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
