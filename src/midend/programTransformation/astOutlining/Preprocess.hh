/**
 *  \file Preprocess.cc
 *  \brief Outlining preprocessor.
 *
 *  This module implements functionality to transform an arbitrary
 *  outlineable statement into a canonical form that simplifies the
 *  core outlining algorithm.
 */

#if !defined(INC_LIAOUTLINER_PREPROCESS_HH)
#define INC_LIAOUTLINER_PREPROCESS_HH //!< Preprocess/Preprocess.hh included.

class SgBasicBlock;
class SgVariableDeclaration;
class SgStatement;

namespace Outliner
{
  namespace Preprocess
  {

    //! Sanity check and patch up outliner options
    // Outliner has some options which can be set by users. They have certain dependencies we want
    // to check and enforce. 
    void checkAndPatchUpOptions();
    
    /*!
     *  \brief Preprocesses the outline target and returns an
     *  equivalent SgBasicBlock to outline.
     *
     *  The goal of this preprocessing is to simplify outlining by
     *  always providing the caller with an SgBasicBlock to outline.
     */
    SgBasicBlock* preprocessOutlineTarget (SgStatement* s);

    /*!
     *  \brief Normalizes outline targets that are variable declarations,
     *  returning a new SgBasicBlock to outline.
     *
     *  This routine converts a variable declaration of the form:
     *
     *    TYPE i, j = J0, k = K0, ...;
     *
     *  to:
     *
     *    TYPE i, j, k, ...;
     *    { // @RET
     *      j = J0;
     *      k = K0;
     *      ...
     *    }
     *
     *  and returns a pointer to the SgBasicBlock at "@RET".
     * 
     *  In addition to creating a new SgBasicBlock, this routine modifies
     *  the original SgVariableDeclaration by removing all
     *  SgAssignInitializers.
     */
    SgBasicBlock* normalizeVarDecl (SgVariableDeclaration* s);

    /*!
     *  \brief Convert the "plain-old" statement into an SgBasicBlock.
     *  This normalization simplifies outlining of single statements.
     */
    SgBasicBlock* createBlock (SgStatement* s);

    /*!
     *  \brief Transform '#if' directive structure.
     *  It is possible that certain preprocessor directives may
     *  straddle the boundaries of a basic block. This routine
     *  attempts to detect and transform these instances so that the
     *  block may be safely outlined.
     *
     *  In particular, consider the following SgBasicBlock, which has
     *  2 '#if' directives straddling it (labeled 'B' and 'D') and
     *  one surrounding it ('A').
     *
     *    #if [A1]
     *       S0;
     *    #  if [B1]     <==+
     *       S1;            |
     *    #  elif [B2]   <==+  
     *       S2;            |
     *       {              |
     *          S3;         +--- Straddles opening brace.
     *    #  elif [B3]   <==+
     *          S4;         |
     *    #  endif [B]   <==+
     *          S5;
     *    #  if [C]       <==+
     *          S6;          +-- Fully within the block
     *    #  endif [C]     <==+
     *          S7;
     *    #  if [D1]    <==+
     *          S8;        |
     *    #  elif [D2]  <==+
     *          S9;        +---- Straddles ending brace.
     *       }             |
     *       S10;          |
     *    #  elif [D3]  <==+
     *       S11;          |
     *    #  endif [D]  <==+
     *       S12;
     *    #endif [A]
     *
     *  This routine will transform this block into (new statements
     *  and directives marked with a '**'):
     *
     *    #if [A1]
     *       S0;
     *    #  if [B1]
     *       S1;
     *    #  elif [B2]
     *       S2;
     *    #  endif // [B] **  <==+  CLOSE ([B]);
     *    #endif // [A] **    <==+
     *       {
     *          { ** <==============+
     *    #if 1 // [A1] **          |  OPEN ([B]);
     *    #  if 1 // [B1, B2] ** <==+
     *             S3;
     *    #  elif [B3]
     *             S4;
     *    #  endif [B]
     *             S5;
     *    #  if [C]
     *             S6;
     *    #  endif [C]
     *             S7;
     *    #  if [D1]
     *             S8;
     *    #  elif [D2]
     *             S9;
     *    #  endif [D] ** <==+  CLOSE ([D]);
     *    #endif [A] **   <==+
     *          } **
     *    #if 1 //[A1] ** <=========+  OPEN ([D2]);
     *    #  if 1 // [D1, D2] ** <==+
     *       }
     *       S10;
     *    #  elif [D3]
     *       S11;
     *    #  endif [D]
     *       S12;
     *    #endif [A]
     *
     *  Note that the block to-be-outlined is now fully
     *  self-contained. Logically, the transformation consists of
     *  "closing" and "opening" the '#if' directive contexts at the
     *  boundaries to the outlined block as shown above.
     */
    SgBasicBlock* transformPreprocIfs (SgBasicBlock* b);

    /*!
     *  \brief Canonicalize references to 'this'.
     *
     *  Given a basic block with 'this' expressions:
     *
     *    {
     *      ... this ...
     *    }
     *
     *  creates a level of indirection to 'this' through an explicit local
     *  variable, so that the outliner includes it in the outlined
     *  function parameter list:
     *
     *    // 'this' shell
     *    {
     *      TYPEOF(this)* this__ptr__ = (TYPEOF(this) *)this;
     *      {
     *        ... this__ptr__ ...
     *      }
     *    }
     */
    SgBasicBlock* transformThisExprs (SgBasicBlock* b);

    /*!
     *  \brief Preprocess non-local control flow for outlining.
     *
     *  Let 'B' be an input code block (i.e., an SgBasicBlock node). We
     *  define a non-local jump to be any break, continue, return, or goto
     *  statement that cedes control to some point outside 'B'.
     *
     *  Given 'B', of the form,
     *
     *    // Start of input block, 'B'
     *    {
     *      ... // bunch of code
     *      JUMP_1;  // First non-local "jump" statement
     *      ...
     *      JUMP_2;  // Second non-local "jump"
     *      ...
     *      JUMP_k;  // k-th non-local "jump"
     *      ...
     *    }
     *
     *  where each JUMP_i is a non-local jump statement, this routine
     *  transforms 'B' into
     *
     *    // Start of the transformed 'B'
     *    {
     *      int EXIT_TAKEN__ = 0;
     *      // A new block, 'G', transformed to use a local 'goto'.
     *      {
     *        ...
     *        {
     *          EXIT_TAKEN__ = 1;
     *          goto NON_LOCAL_EXIT__;
     *        }
     *        ...
     *        {
     *          EXIT_TAKEN__ = 2;
     *          goto NON_LOCAL_EXIT__;
     *        }
     *        ...
     *        {
     *          EXIT_TAKEN__ = k;
     *          goto NON_LOCAL_EXIT__;
     *        }
     *        ...
     *    NON_LOCAL_EXIT__: ;
     *      }
     *      if (EXIT_TAKEN__ == 1) JUMP_1;
     *      else if (EXIT_TAKEN__ == 2) JUMP_2;
     *      ...
     *      else if (EXIT_TAKEN__ == k) JUMP_k;
     *    }
     *
     *  and returns 'T'.
     */
    SgBasicBlock* transformNonLocalControlFlow (SgBasicBlock* b_orig);

    /*!
     *  \brief Gathers non-local declarations into the local scope.
     *
     *  This routine analyzes the basic block to-be-outlined, and
     *  determines if any external function declarations need to be
     *  copied into the block's scope. This transformation ensures
     *  that the basic block can be a "standalone" body for the
     *  outlined function.
     */
    void gatherNonLocalDecls (SgBasicBlock* b);

  } /* namespace Preprocess */
} /* namespace Outliner */

#endif

// eof
