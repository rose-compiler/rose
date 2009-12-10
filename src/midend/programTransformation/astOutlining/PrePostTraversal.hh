/*!
 *  \file PrePostTraversal.hh
 *
 *  \brief Implements a top-down/bottom-up traversal without explicit
 *  attributes, i.e., implements a variation on the simple traversal
 *  that exposes the top-down and bottom-up visits.
 */

#if !defined(INC_PREPOSTTRAVERSAL_HH)
#define INC_PREPOSTTRAVERSAL_HH //!< PrePostTraversal.hh included.



namespace ASTtools
{
  class PrePostTraversal
    : public AstTopDownBottomUpProcessing<_DummyAttribute, _DummyAttribute>
  {
  public:
    //! Called as we descend the tree.
    virtual void visitTopDown (SgNode *) = 0;
    
    //! Called as we ascend the tree.
    virtual void visitBottomUp (SgNode *) = 0;

    //! \name Traversal routines.
    //@{
    virtual void traverse (SgNode *);
    virtual void traverseWithinFile (SgNode *);
    //@}
    
  private:
    // Hide the usual attribute evaluation routines.
    virtual _DummyAttribute
    evaluateInheritedAttribute (SgNode* node, _DummyAttribute);
    
    virtual _DummyAttribute
    evaluateSynthesizedAttribute (SgNode *, _DummyAttribute,
                                  SynthesizedAttributesList);
  };
}

#endif // !defined(INC_PREPOSTTRAVERSAL_HH)

// eof
