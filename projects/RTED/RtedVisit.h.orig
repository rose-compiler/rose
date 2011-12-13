#ifndef RTEDVISITVAR_H
#define RTEDVISITVAR_H

#include "RtedTransformation.h"

// Build an inherited attribute for the tree traversal to test the rewrite mechanism

namespace rted
{
  struct InheritedAttribute
  {
    bool             isAssignInitializer;
    bool             isVariableDecl;
    bool             isArrowExp;
    bool             isAddressOfOp;
    ReturnInfo::Kind functionReturnType;
    size_t           openBlocks;
    SgStatement*     lastGForLoop;
    SgBinaryOp*      lastBinary;
    size_t           lastBreakableStmtLevel;
    size_t           lastContinueableStmtLevel;

    InheritedAttribute()
    : isAssignInitializer(false), isVariableDecl(false),
      isArrowExp(false), isAddressOfOp(false),
      functionReturnType(ReturnInfo::rtNone),
      openBlocks(0), lastGForLoop(NULL), lastBinary(NULL),
      lastBreakableStmtLevel(0), lastContinueableStmtLevel(0)
    {}
  };

#if 0
  struct EvaluatedAttribute
  {
    bool instrument_function_call;

    EvaluatedAttribute()
    : instrument_function_call(false)
    {}

    EvaluatedAttribute& operator+=(const EvaluatedAttribute& rhs)
    {
      instrument_function_call = instrument_function_call || rhs.instrument_function_call;
      return *this;
    }

    EvaluatedAttribute operator+(const EvaluatedAttribute& rhs) const
    {
      EvaluatedAttribute res(*this);

      res += rhs;
      return res;
    }
  };
#endif

  struct VariableTraversal : public AstTopDownProcessing<InheritedAttribute>
  {
<<<<<<< HEAD
      typedef SgTopDownProcessing<InheritedAttribute> Base;
=======
     typedef AstTopDownProcessing<InheritedAttribute> Base;
>>>>>>> master

      explicit
      VariableTraversal(RtedTransformation* t);

      // Functions required
      InheritedAttribute evaluateInheritedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute);
      // EvaluatedAttribute evaluateSynthesizedAttribute(SgNode* astNode, InheritedAttribute inh, SynthesizedAttributesList synList);

      friend class InheritedAttributeHandler;
      // friend class EvaluatedAttributeHandler;

<<<<<<< HEAD
    private:
      RtedTransformation* const transf;
=======
  private:
    RtedTransformation* const   transf;
>>>>>>> master
      SourceFileType      current_file_type;  // could be part of inherited attribute

      // should fail when needed
      VariableTraversal();
      VariableTraversal(const VariableTraversal&);
      VariableTraversal& operator=(const VariableTraversal&);
  };
}

#endif
