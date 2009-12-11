/*!
 *  \file CPreproc/IfDirectiveContextFinder.hh
 *
 *  \brief Implements a module to determine the '#if' directive
 *  context surrounding a specified target node.
 */

#if !defined(INC_CPREPROC_IFDIRECTIVECONTEXTFINDER_HH)
//! CPreproc/IfDirectiveContextFinder.hh included.
#define INC_CPREPROC_IFDIRECTIVECONTEXTFINDER_HH

#include <IfDirectiveExtractor.hh>

namespace CPreproc
{
  /*!
   *  \brief Find the '#if' case surrounding a given node.
   *
   *  Given a target node, this traversal extracts the full program's
   *  '#if' directive structure and determines which '#if' case
   *  surrounds the top and bottom portion of the target node.
   *
   *  For example, suppose a target SgBasicBlock is situated in the
   *  source code as follows:
   *
   *    // ...
   *    #elif CONDITION_1
   *    // ...
   *    {
   *      // ...
   *    #elif CONDITION_2
   *      // ...
   *    }
   *    // ...
   *
   *  Then, this traversal permits the caller to determine that '#elif
   *  CONDITION_1' guards the top of the block, and '#elif CONDITION_2'
   *  guards the bottom of the block.
   */
  class IfDirectiveContextFinder : public IfDirectiveExtractor
  {
  public:
    IfDirectiveContextFinder (CPreproc::Ifs_t& ifs,
                              const SgLocatedNode* target);
    virtual void visitTopDown (SgNode* n);
    virtual void visitBottomUp (SgNode* n);

    //! \name Get context.
    //@{
    If::Case* getContextTop (void);
    const If::Case* getContextTop (void) const;
    If::Case* getContextBottom (void);
    const If::Case* getContextBottom (void) const;
    //@}

  private:
    //! Node for which context is being sought.
    const SgLocatedNode* target_;
    
    //! Stores the context at the top of the target node.
    If::Case* top_;
    
    //! Stores the context at the bottom of the target node.
    If::Case* bottom_;
  }; // class IfDirectiveContextFinder
} // namespace CPreproc

#endif // !defined(INC_CPREPROC_IFDIRECTIVECONTEXTFINDER_HH)

// eof
