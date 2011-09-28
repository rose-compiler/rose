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
    bool          isVariableDecl;
    bool          isArrowExp;
    bool          isAddressOfOp;
    bool          isForStatement;
    size_t        openBlocks;
    SgStatement*  lastGForLoop;
    SgBinaryOp*   lastBinary;

    InheritedAttribute()
    : function(false),  isAssignInitializer(false), isVariableDecl(false),
      isArrowExp(false), isAddressOfOp(false), isForStatement(false),
      openBlocks(0), lastGForLoop(NULL), lastBinary(NULL)
    {}
  };


  struct VariableTraversal : public AstTopDownProcessing<InheritedAttribute>
  {
     typedef AstTopDownProcessing<InheritedAttribute> Base;

     explicit
     VariableTraversal(RtedTransformation* t) ;

       // Functions required
    InheritedAttribute evaluateInheritedAttribute (SgNode* astNode, InheritedAttribute inheritedAttribute );

    friend class InheritedAttributeHandler;

  private:
    RtedTransformation* const   transf;
      SourceFileType      current_file_type;  // could be part of inherited attribute

    // should fail when needed
    VariableTraversal();
    VariableTraversal(const VariableTraversal&);
    VariableTraversal& operator=(const VariableTraversal&);
  };
}

#endif
