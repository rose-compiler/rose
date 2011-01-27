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
		 typedef SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute> Base;

     public:

		 typedef std::vector<SgStatement*>                                            LoopStack;
		 typedef std::vector<SgBinaryOp*>                                             BinaryOpStack;


   explicit
   VariableTraversal(RtedTransformation* t) ;

   ~VariableTraversal()
	 {
		 ROSE_ASSERT(for_loops.empty());
		 ROSE_ASSERT(binary_ops.empty());
	 }

     // Functions required
     InheritedAttribute evaluateInheritedAttribute (
						    SgNode* astNode,
						    InheritedAttribute inheritedAttribute );

     SynthesizedAttribute evaluateSynthesizedAttribute (
							SgNode* astNode,
							InheritedAttribute inheritedAttribute,
							SubTreeSynthesizedAttributes synthesizedAttributeList );

	  friend class InheritedAttributeHandler;

		 private:
       RtedTransformation* const   transf;
			 BinaryOpStack               binary_ops;  ///< stores all binary operations in the current traversal
			 LoopStack                   for_loops;   ///< stores C/C++ for and UPC upc_forall

			 // internal functions
			 void handleIfVarRefExp(SgVarRefExp* varref, const InheritedAttribute& inh);

		   // should fail when needed
		   VariableTraversal();
			 VariableTraversal(const VariableTraversal&);
			 VariableTraversal& operator=(const VariableTraversal&);
   };

#endif
