/**
 *  \file Outliner.hh
 *
 *  \brief An outlining implementation.
 *
 *  \author Chunhua Liao <liaoch@llnl.gov>, Richard Vuduc
 *  <richie@llnl.gov>
 *
 *  This implementation is based largely on the code by Liao for the
 *  ROSE OpenMP_Translator project. Vuduc extended the code to handle
 *  additional cases and use an alternative calling convention for
 *  functions.
 *
 *  \todo Outline: arbitrary lists of statements taken from a single
 *  SgBasicBlock.
 */

#if !defined(INC_LIAOUTLINER_HH)
#define INC_LIAOUTLINER_HH

#include <cstdlib>
#include <vector>
#include <string>

//! \name Forward declarations to relevant Sage classes.
//@{
class SgProject;
class SgFunctionDeclaration;
class SgStatement;
class SgPragmaDeclaration;
//@}

namespace Outliner
{
  //! A set of flags to control the internal behavior of the outliner
  //classic outlining behavior: 
  //side effect analysis for pass-by-value and pass-by-ref, reuse parameters
  extern bool enable_classic; 
  // use a wrapper for all variables or one parameter for a variable or a wrapper for all variables
  extern bool useParameterWrapper;  // use a wrapper for parameters of the outlined function
                   // turned on by command line option:   -rose:outline:parameter_wrapper
  extern bool preproc_only_;  // preprocessing only, -rose:outline:preproc-only
  extern bool useNewFile; // Generate the outlined function into a separated new source file
                          // -rose:outline:new_file
  extern std::vector<std::string> handles;   // abstract handles of outlining targets, given by command line option -rose:outline:abstract_handle for each
  extern bool temp_variable; // Use temporary variables to reduce the uses of pointer dereferencing. Activated by -rose:outline:temp_variable
  extern bool enable_debug; // output debug information for outliner
  extern bool exclude_headers; // exclude headers from the new file containing outlined functions

  //! Accept a set of command line options to adjust internal behaviors
  // Please use this function before calling the frontend() to set the internal flags
  void commandLineProcessing(std::vector<std::string> &argvList);
  //
  //! Returns true iff the statement is "outlineable."
  bool isOutlineable (const SgStatement* s, bool verbose = false);

  //! Stores the main results of an outlining transformation.
  struct Result
  {
    //! The outlined function's declaration and definition.
    SgFunctionDeclaration* decl_;

    //! A call statement to invoke the outlined function.
    SgStatement* call_;

    //! A SgFile pointer to the newly generated source file containing the
    // outlined function if -rose:outline:new_file is specified (useNewFile==true)
    SgFile* file_;

    Result (void); //! Sets all fields to 0
    Result (SgFunctionDeclaration *, SgStatement *, SgFile* file=NULL);
    Result (const Result&); //! Copy constructor.
    ~Result (void) {}; //! Shallow; does not delete fields.
    bool isValid (void) const; //! Returns true iff result is usable
  };

  /*!
   *  \brief Create a unique outlined-function name for the specified
   *  statement.
   *
   *  The generated name will be "unique" within the current
   *  translation unit, and is likely (but not guaranteed) to be
   *  unique across a project.
   */
  std::string generateFuncName (const SgStatement* stmt);

  /*!
   *  \brief Create a unique outlined-function's wrapper argument name for the specified
   *  statement.
   *
   *  The generated name will be "unique" within the current
   *  translation unit, and is likely (but not guaranteed) to be
   *  unique across a project.
   */
  std::string generateFuncArgName (const SgStatement* stmt);

  //! Outlines the given statement. The main programming interface.
  /*!
   *  This function pre-process the target stmt first and 
   *  outlines the specified statement, s. It creates a
   *  new outlined function definition, f, inserts f into the first
   *  scope surrounding s that may contain a function (or member
   *  function) definition, replaces s with a call to f, and finally
   *  returns f.
   *
   *  It can also does pre-processing only if directed by the internal flag,
   *  which is specified by user command line option and processed by 
   *  commandLineProcessing();
   *
   *  Programmers are expected to tell if a statement is outlineable before
   *  calling this function.
   */
  Result outline (SgStatement* s);

  //! Outline to a new function with the specified name.
  Result outline (SgStatement* s, const std::string& func_name);

  //! If 's' is an outline pragma, this function "executes" it.
  /*!
   *  \post The outlined statement and the pragma are removed from the
   *  AST.
   */
  Result outline (SgPragmaDeclaration* s);

  //! Outlines all regions marked by outlining pragmas.
  /*!
   *  \returns The number of outline directives processed.
   */
  size_t outlineAll (SgProject *);

  /**
   * \name The following routines, intended for debugging, mirror the
   * core outlining routines above, but only run the outlining
   * preprocessing phase, returning the outlineable statement if
   * present.
   */
  //@{
  SgBasicBlock* preprocess (SgStatement* s);
  SgBasicBlock* preprocess (SgPragmaDeclaration* s);
  size_t preprocessAll (SgProject *);
  //@}
};

#endif // !defined(INC_LIAOUTLINER_HH)

// eof
