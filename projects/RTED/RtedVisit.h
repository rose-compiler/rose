#ifndef RTEDVISITVAR_H
#define RTEDVISITVAR_H

#include "RtedTransformation.h"

// Build an inherited attribute for the tree traversal to test the rewrite mechanism

namespace rted
{
  struct InheritedAttribute
  {
    bool          function;
    bool          isAssignInitializer;
    bool          isArrowExp;
    bool          isAddressOfOp;
    bool          isForStatement;

    InheritedAttribute()
    : function(false),  isAssignInitializer(false), isArrowExp(false),
      isAddressOfOp(false), isForStatement(false)
    {}
  };

  struct SynthesizedAttribute
  {
#if NOT_YET_IMPLEMENTED
    typedef void (RtedTransformation::*Modop(SgExpression&, SgFunctionSymbol&));

    const char* operation;
    Modop*      handler;

    SynthesizedAttribute()
    : operation(NULL), handler(NULL)
    {}

    void modify(RtedTransformation& trans, SgExpression& n, SgFunctionSymbol& c)
    {
      // trans.*handler(n, c);
    }
#endif /* NOT_YET_IMPLEMENTED */
  };

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

       if (!binary_ops.empty())
       {
         std::cerr << "still " << binary_ops.size() << " inside" << std::endl;
         std::cerr << "front = " << binary_ops.front()->unparseToString() << std::endl;
         std::cerr << "back = " << binary_ops.back()->unparseToString() << std::endl;
       }

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
}

#endif
