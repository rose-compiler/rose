/**
 *  \file ASTtools/PreprocessingInfo.hh
 *
 *  \brief Higher-level wrappers related to query and manipulation of
 *  PreprocessingInfo objects.
 *
 *  \author Richard Vuduc <richie@llnl.gov>
 */

#if !defined(INC_ASTTOOLS_PREPROCESSINGINFO_HH)
//! ASTtools/PreprocessingInfo.hh included.
#define INC_ASTTOOLS_PREPROCESSINGINFO_HH

#include <iostream>


namespace ASTtools
{
  //! Returns true if info is positioned before its statement.
  bool isPositionBefore (const PreprocessingInfo* info);

  //! Returns true if info is positioned after its statement.
  bool isPositionAfter (const PreprocessingInfo* info);

  //! Returns true if info is positioned inside its statement.
  bool isPositionInside (const PreprocessingInfo* info);

  //! Returns true if the info is an '#if', '#ifdef', or '#ifndef'.
  bool isIfDirectiveBegin (const PreprocessingInfo* info);

  //! Returns true if the info is an '#elif' or '#else'.
  bool isIfDirectiveMiddle (const PreprocessingInfo* info);

  // !Returns true if the info is an '#endif'.
  bool isIfDirectiveEnd (const PreprocessingInfo* info);
  //! Attaches a string as a new C++-style comment to the given statement.
  void attachComment (const std::string& comment, SgStatement* s);

  //! Attaches a string as a new C++-style comment to the given statement.
  void attachComment (const char* comment, SgStatement* s);

  //! Inserts an '#include' of a header file at the first statement.
  bool insertHeader (const std::string& filename, SgProject* proj);

  /*!
   *  \brief Returns the preprocessing information list attached to
   *  the given statement; if no preprocessing information exists,
   *  then this routine will create an empty list and return its
   *  pointer. Thus, this routine is guaranteed to return a non-NULL
   *  result.
   */
  AttachedPreprocessingInfoType* createInfoList (SgStatement* s);

  //! Removes all preprocessing information at a given position.
  void cutPreprocInfo (SgBasicBlock* b,
                       PreprocessingInfo::RelativePositionType pos,
                       AttachedPreprocessingInfoType& save_buf);

  //! Pastes preprocessing information at the front of a statement.
  void pastePreprocInfoFront (AttachedPreprocessingInfoType& save_buf,
                              SgStatement* s);

  //! Pastes preprocessing information at the back of a statement.
  void pastePreprocInfoBack (AttachedPreprocessingInfoType& save_buf,
                             SgStatement* s);

  /*!
   *  \brief Moves 'before' preprocessing information.
   *
   *  Moves all preprocessing information attached 'before' the source
   *  statement to the front of the destination statement.
   */
  void moveBeforePreprocInfo (SgStatement* src, SgStatement* dest);

  /*!
   *  \brief Moves 'inside' preprocessing information.
   *
   *  Moves all preprocessing information attached 'inside' the source
   *  block to be inside the destination block.
   */
  void moveInsidePreprocInfo (SgBasicBlock* src, SgBasicBlock* dest);

  /*!
   *  \brief Moves 'after' preprocessing information.
   *
   *  Moves all preprocessing information attached 'after' the source
   *  statement to the end of the destination statement.
   */
  void moveAfterPreprocInfo (SgStatement* src, SgStatement* dest);

  //! Moves preprocessingInfo of stmt2 to stmt1.
  void moveUpPreprocInfo (SgStatement* stmt1, SgStatement* stmt2);

  //! Dumps a statement's preprocessing information.
  void dumpPreprocInfo (const SgStatement* s, std::ostream& o);

} // namespace ASTtools

#endif

// eof
