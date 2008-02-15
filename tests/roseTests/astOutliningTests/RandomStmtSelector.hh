/*!
 *  \file RandomStmtSelector.hh
 *
 *  \brief Implements a routine that can select a random number of
 *  outlineable statements from a project.
 */

#if !defined(INC_RANDOMSTMTSELECTOR_HH)
#define INC_RANDOMSTMTSELECTOR_HH //!< RandomStmtSelector.hh included.

namespace RandomStmtSelector
{
  //! Stores a set of statements.
  typedef std::set<SgStatement *> StmtSet_t;

  /*!
   *  Selects up to the specified number of outlineable statements
   *  from the given project.
   */
  void selectOutlineable (SgProject* proj, size_t max_stmts,
                          StmtSet_t& stmts);
}

#endif // !defined(INC_RANDOMSTMTSELECTOR_HH)

// eof
