/*!
 *  \file CPreproc/IfDirectiveExtractor.hh
 *
 *  \brief Implements a module to extract the '#if' directive
 *  structure of a program.
 */

#if !defined(INC_CPREPROC_IFDIRECTIVEEXTRACTOR_HH)
//! CPreproc/IfDirectiveExtractor.hh included.
#define INC_CPREPROC_IFDIRECTIVEEXTRACTOR_HH

#include <stack>
#include <PrePostTraversal.hh>
#include <If.hh>

namespace CPreproc
{
  //! Traversal to extract '#if' directives.
  class IfDirectiveExtractor : public ASTtools::PrePostTraversal
  {
  public:
    IfDirectiveExtractor (Ifs_t& ifs);
    virtual void visitTopDown (SgNode* n);
    virtual void visitBottomUp (SgNode* n);

  protected:
    //! Provides child classes with access to current '#if' directive.
    //@{
    const If::Case* getCurrentContext (void) const;
    If::Case* getCurrentContext (void);
    //@}
    
  private:
    //! Collection of all top-level '#if' directives processed.
    Ifs_t& ifs_;
    
    //! Stack of partially complete '#if' directives.
    std::stack<If *> pending_;

    //! If the stack is empty, aborts with a message.
    void validateNonEmptyStack (const PreprocessingInfo* info,
                                const SgLocatedNode* node) const;

    //! Visits a node, either on the top-down or bottom-up visit.
    void visit (SgNode* n, bool isTopDown);

    /*!
     *  Updates the tree structure for a newly encountered 'info'
     *  directive.
     */
    void handleCase (PreprocessingInfo* info, SgLocatedNode* node);

    //! Creates a new '#if' directive.
    void newIf (PreprocessingInfo* info, SgLocatedNode* node);

    //! Adds the given directive/node to the current '#if' case.
    void addCase (PreprocessingInfo* info, SgLocatedNode* node);

    //! Asserts that we are not trying to add a case to a terminated '#if'.
    static void checkLastCaseNotEndif (const If* i);
  }; // class IfDirectiveExtractor
} // namespace CPreproc

#endif // !defined(INC_CPREPROC_IFDIRECTIVEEXTRACTOR_HH)

// eof
