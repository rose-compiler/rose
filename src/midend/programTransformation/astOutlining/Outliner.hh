/**
 *  \file Outliner.hh
 *
 *  \brief An outlining implementation.
 *
 *  \author Chunhua Liao <liaoch@llnl.gov>, Richard Vuduc <richie@llnl.gov>
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
#include <set>
#include <string>
#include <ASTtools.hh>
#include <VarSym.hh>
#include <Sawyer/CommandLine.h>
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
  //
  // Default behavior
  // ----------------------------------
  // if none of the flags are set to true, the default behavior is:
  // * all variables are passed by references whenever possible by default (pointer types, and pointer dereferencing)
  // * function parameter packing is needed to transfer the value of parameter to a local declaration
  
  // default behavior + variable cloning 
   // Using variable cloning(temp variable) to reduce the uses of pointer dereferencing in computation kernels.
  // Details are in the paper: Chunhua Liao, Daniel J. Quinlan, Richard Vuduc, and Thomas Panas. 2009. Effective source-to-source outlining to support whole program empirical optimization. In Proceedings of the 22nd international conference on Languages and Compilers for Parallel Computing (LCPC'09).
  ROSE_DLL_API extern bool temp_variable; // Use temporary variables to reduce the uses of pointer dereferencing. Activated by -rose:outline:temp_variable

  // -----------------------------------
  // Method 1: classic outlining behavior: 
  // Each parameter represents a single variable being passed in/out the outlined function. No parameter wrapping
  // Use parameters of the outlined function directly in the function,  no local declaration/copying of parameter is needed .
  // Side effect analysis is used for deciding on pass-by-value (readOnly) and pass-by-ref, 
  ROSE_DLL_API extern bool enable_classic; 
  // -----------------------------------
  // Method 2: using a wrapper (array of pointers vs. structure of flexible typed members)
  // use a wrapper for all variables or one parameter for a variable or a wrapper for all variables
  ROSE_DLL_API extern bool useParameterWrapper;  // use an array of pointers wrapper for parameters of the outlined function. all things are passed by pointers (addressOf) by default
  ROSE_DLL_API extern bool useStructureWrapper;  // use a structure-type wrapper for parameters of the outlined function, this is a sub option for useParameterWrapper. false means using array, true means using structure (the same as useParameterWrapper).  
  // turned on by command line option:   -rose:outline:parameter_wrapper
                   
 
  //---------------------------------others ---------------------
  ROSE_DLL_API extern bool preproc_only_;  // preprocessing only, -rose:outline:preproc-only
  ROSE_DLL_API extern bool useNewFile; // Generate the outlined function into a separated new source file
                          // -rose:outline:new_file
  ROSE_DLL_API extern bool copy_origFile; // when generating the new file to store outlined function, copy entire original file to it.
  ROSE_DLL_API extern std::vector<std::string> handles;   // abstract handles of outlining targets, given by command line option -rose:outline:abstract_handle for each
  ROSE_DLL_API extern bool enable_debug; // output debug information for outliner
  ROSE_DLL_API extern bool exclude_headers; // exclude headers from the new file containing outlined functions
  ROSE_DLL_API extern bool enable_liveness; // enable liveness analysis to reduce restoring statements when temp variables are used
  ROSE_DLL_API extern bool use_dlopen; // Outlining the target to a separated file and calling it using a dlopen() scheme. It turns on useNewFile.

  ROSE_DLL_API extern bool enable_template;  // Enabling outlining code blocks inside template functions
  
  ROSE_DLL_API extern std::string output_path; // where to save the new file containing the outlined function

// DQ (3/19/2019): Suppress the output of the #include "autotuning_lib.h" since some tools will want to define there own supporting libraries and header files.
  ROSE_DLL_API extern bool suppress_autotuning_header; // when generating the new file to store outlined function, suppress output of #include "autotuning_lib.h".

  //! Constants used during translation
  // A support lib's header name
  const std::string AUTOTUNING_LIB_HEADER="autotuning_lib.h";
  // the lib function call to find a specified function pointer
  const std::string FIND_FUNCP_DLOPEN="findFunctionUsingDlopen";
  const std::string DEFAULT_OUTPUT_PATH="/tmp";

  struct DeferedTransformation
     {
    // DQ (8/7/2019): Store data required to support defering the transformation to insert the outlined function prototypes 
    // into class declaration (when this is required to support the outlined function's access to protected or private data members).
    // This is part of an optimization to support the optimization of header file unparsing (limiting the overhead of supporting any 
    // header file to just focus on the few (typically one) header file that would have to be unparsed.

       SgClassDefinition* class_definition;
       SgDeclarationStatement* target_class_member;
       SgDeclarationStatement* new_function_prototype;

       typedef std::set<SgClassDefinition *> ClassDefSet_t;
       ClassDefSet_t targetClasses;

       typedef std::vector<SgFunctionDeclaration *> FuncDeclList_t;
       FuncDeclList_t targetFriends;

    // DQ (12/5/2019): Added ROSE_DLL_API prefix for Windows support (too all of these functions).
       ROSE_DLL_API DeferedTransformation();
       ROSE_DLL_API DeferedTransformation(SgClassDefinition* class_definition, SgDeclarationStatement* target_class_member, SgDeclarationStatement* new_function_prototype);
       ROSE_DLL_API DeferedTransformation (const DeferedTransformation& X); //! Copy constructor.
       ROSE_DLL_API ~DeferedTransformation (void); //! Shallow; does not delete fields.

       ROSE_DLL_API DeferedTransformation & operator= (const DeferedTransformation& X); //! operator=()
     };

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

 // DQ (8/7/2019): Store data required to support defering the transformation to insert the outlined function prototypes 
 // into class declaration (when this is required to support the outlined function's access to protected or private data members).
    SgDeclarationStatement* target_class_member;
    SgDeclarationStatement* new_function_prototype;

 // DQ (8/15/2019): Adding support to defere the transformations in header files (a performance improvement).
    DeferedTransformation deferedTransformation;

 // DQ (12/5/2019): Added ROSE_DLL_API prefix for Windows support (too all of these functions).
    ROSE_DLL_API Result (void); //! Sets all fields to 0

  // DQ (8/15/2019): Adding support to defere the transformations in header files (a performance improvement).
 // Result (SgFunctionDeclaration *, SgStatement *, SgFile* file=NULL);
    ROSE_DLL_API Result (SgFunctionDeclaration *, SgStatement *, SgFile* file, DeferedTransformation deferedTransformation);

    ROSE_DLL_API Result (const Result&); //! Copy constructor.
    ROSE_DLL_API ~Result (void) {}; //! Shallow; does not delete fields.
    ROSE_DLL_API bool isValid (void) const; //! Returns true iff result is usable
  };

  /** Description of outliner command-line.
   *
   *  This function creates a description of command-line switches that are recognized by the outliner. This description can be
   *  incorporated into a @ref Sawyer::CommandLine::Parser by tools that need to create a parser that recognizes outliner
   *  switches. When the command-line is parsed with these descriptions, various settings in this object are adjusted. */
ROSE_DLL_API Sawyer::CommandLine::SwitchGroup commandLineSwitches();

  /** Validate and adjust analysis settings.
   *
   *  This function should be called after changing analysis settings (directly or via command-line parsing) to ensure
   *  they are all in a consistent state before any analysis is performed.
   *
   *  @todo Ideally this should happen automatically by those functions that perform analysis so the user doesn't have to
   *  remember this, but I'm not familiar enough which the code to know which functions those are.  Originally, the outliner
   *  only validated its settings when they were changed by its @ref commandLineProcessing function, which still happes. [Robb
   *  P Matzke 2016-09-11] */
  ROSE_DLL_API void validateSettings();

  //! Accept a set of command line options to adjust internal behaviors
  // Please use this function before calling the frontend() to set the internal flags
  ROSE_DLL_API void commandLineProcessing(std::vector<std::string> &argvList);
  //
  //! Returns true iff the statement is "outlineable."
  ROSE_DLL_API bool isOutlineable (const SgStatement* s, bool verbose = false);


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
  ROSE_DLL_API Result outline (SgStatement* s);

  //! Outline to a new function with the specified name, calling preprocessing internally
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
  ROSE_DLL_API size_t outlineAll (SgProject *);

  /**
   * \name The following routines, intended for debugging, mirror the
   * core outlining routines above, but only run the outlining
   * preprocessing phase, returning the outlineable statement if
   * present.
   */
  //@{
  ROSE_DLL_API SgBasicBlock* preprocess (SgStatement* s);
  ROSE_DLL_API SgBasicBlock* preprocess (SgPragmaDeclaration* s);
  ROSE_DLL_API size_t preprocessAll (SgProject *);
  //@}
  
   /*!
     *  \brief Outlines the given basic block into a function named
     *  'name'.
     *
     *  This routine performs the outlining transformation, including
     *  insertion of the new outlined-function declaration and call.
     */
    Result outlineBlock (SgBasicBlock* b, const std::string& name);

    /*!
     *  \brief Computes the set of variables in 's' that need to be
     *  passed to the outlined routine (semantically equivalent to shared variables in OpenMP) 
     *
     *  Note: user codes should handle non-generic variable handling on their own.
     *  Outliner only handles generic operations.
     *  For example, in OpenMP translation calling outliner: OpenMP specific variables 
     *  such as private, firstprivate, reduction variables are handled by OmpSupport::transOmpVariables() 
     *  before it calls outliner. So the outliner can focus on generic operations while the caller should
     *  handle their special variables in advance. 
     */
    void collectVars (const SgStatement* s, ASTtools::VarSymSet_t& syms);
    //void collectVars (const SgStatement* s, ASTtools::VarSymSet_t& syms, ASTtools::VarSymSet_t& private_syms);

    /*!\brief Generate a new source file under the same SgProject as
     * target, the file's base name is file_name_str. Suffix is automatically
     * generated according to the file suffix of s
     */
    ROSE_DLL_API SgSourceFile* generateNewSourceFile(SgBasicBlock* target, const std::string& file_name);

    /*!\brief Obtain the file handle to the separated source file storing outlined functions.  
     * This file will be compiled to .so dynamically loadable library.
     * target is the input code block for outlining. It provides SgProject and input file name info. 
     * the lib source file's name convention is rose_input_lib.[c|cxx].
     */
    ROSE_DLL_API SgSourceFile* getLibSourceFile(SgBasicBlock* target);
    
    // DQ (3/20/2019): This function operates on the new file used to support outlined function definitions.
    /* \brief XXX 
     * This function operates on the new file used to support outlined function definitions.
     * We use a copy of the file where the code will be outlined FROM, so that if there are references to
     * declarations in the outlined code we can support the outpiled code with those references.  This
     * approach has the added advantage of also supporting the same include file tree as the original 
     * file where the outlined code is being taken from.
     */
    // ROSE_DLL_API void convertFunctionDefinitionsToFunctionPrototypes(SgNode* node);
    
    /*!\brief the lib source file's name convention is rose_input_lib.[c|cxx].
     * 
     * target is the input code block for outlining. It provides SgProject and input file name info. 
     * 
     */
    ROSE_DLL_API std::string generateLibSourceFileName (SgBasicBlock* target);

    /*!\brief Generate a struct declaration to wrap all variables to be passed to the outlined function
     * There are two ways to wrap a variable: Using its value vs. Using its address (pointer type)
     *
     * This function will also insert the declaration inside the global scope point right before the outlining target
     * If the scope of the outlined function is different, a declaration will also be inserted there. 
     */
    ROSE_DLL_API
    SgClassDeclaration* generateParameterStructureDeclaration(
        SgBasicBlock* s, // the outlining target
        const std::string& func_name_str, // the name for the outlined function, we generate the name of struct based on this.
        const ASTtools::VarSymSet_t& syms, // variables to be passed as parameters
        ASTtools::VarSymSet_t& pdSyms, // variables must use pointer types (pointer dereferencing: pdf). The rest variables use pass-by-value
        SgScopeStatement* func_scope ); // the scope of the outlined function, could be in another file


    /*!
     *  \brief Returns a new outlined function containing a deep-copy
     *  of s.
     *
     *  This function only creates and returns an outlined function
     *  definition, f. Although it sets the scope of 'f' to be the
     *  first scope surrounding 's' that may contain a function (or
     *  member function) definition, it does not insert 'f' into that
     *  scope.
     *
     *  This function is an "inner" outlining routine which does not
     *  properly handle non-local control flow. To outline statements
     *  containing non-local flow, a caller should instead call \ref
     *  Outliner::outline(), which preprocesses non-local control
     *  flow appropriately. See \ref
     *  Outliner::transformNonLocalControlFlow() for more details.
     *
     *  pdSyms is used to store symbols which must be replaced with 
     *  their corresponding pointer dereferencing if replaced during 
     *  outlining. Used to support -rose:outline:temp_variable
     *       It is also used to support struct wrapper parameter to indicate
     *       if a variable is stored by its address (not value) in the wrapper. 
     *
     * // pSyms are OpenMP private variables, or dead variables (neither livein nor liveout)
     *
     *  Note: private, firstprivate, reduction variables are handled by OmpSupport::transOmpVariables()
     *  They are not in actual use anymore.
     *
     *  \pre The statement does not contain non-local control flow.
     */
 // DQ (2/25/2009): Modified function interface to pass "SgBasicBlock*" as not const parameter.
 // SgFunctionDeclaration* generateFunction (const SgBasicBlock* s,const std::string& func_name_str,const ASTtools::VarSymSet_t& syms,SgScopeStatement* scope);
    ROSE_DLL_API
    SgFunctionDeclaration*
    generateFunction (SgBasicBlock* s,
                      const std::string& func_name_str,
                      const ASTtools::VarSymSet_t& syms,
                      const ASTtools::VarSymSet_t& pdSyms, // variables using their addresses as parameters of the outlined functions, compared to use their values (pass-by-value) 
                      //const std::set<SgInitializedName*>& readOnlyVars, // optional readOnlyVariables to optimize type of parameter when no wrapper is used.
                      //const std::set< SgInitializedName *>& liveOuts, // optional live out variables to optimize away the copy-back statements in variable cloning
                      const std::set< SgInitializedName *>& restoreVars, // variables need to be restored after their clones finish computation
                      SgClassDeclaration* struct_decl, /*optional struct type to wrap parameters*/
                      SgScopeStatement* scope);

     //! Generate packing (wrapping) statements for the variables to be passed 
     //return the unique wrapper parameter for the outlined function
     //target is the outlining target
    ROSE_DLL_API std::string generatePackingStatements(SgStatement* target, ASTtools::VarSymSet_t & syms,  ASTtools::VarSymSet_t & pdsyms, SgClassDeclaration* struct_decl = NULL);

    /*!
     *  \brief Inserts an outlined-function declaration into global scope.
     *
     *  The caller specifies the global scope into which this routine will
     *  insert the declaration. This is needed since we support inserting into
     *  the original global scope and a global scope in a new file.
     *
     *  The caller also provides the original target to be outlined
     *  This information is used to insert the prototypes into the correct places in
     *  the AST.
     */
 // DQ (8/15/2019): Adding support to defer the transformations to header files.
 // ROSE_DLL_API void insert (SgFunctionDeclaration* func, SgGlobal* scope, SgBasicBlock* target_outlined_code )
    ROSE_DLL_API DeferedTransformation insert (SgFunctionDeclaration* func, SgGlobal* scope, SgBasicBlock* outlining_target );

    /*!
     *  \brief Generates a function call parameter list using a set of symbols
     */
    void appendIndividualFunctionCallArgs (const ASTtools::VarSymSet_t& syms, // all symbols for parameters 
                                           const std::set<SgInitializedName*> varUsingOriginalType, // symbols which should be passed using their original types
                                           SgExprListExp* e_list); // the result expression list
    /*!
     *  \brief Generates a call to an outlined function.
     */
    SgStatement* generateCall (SgFunctionDeclaration* out_func,
                              const ASTtools::VarSymSet_t& syms,
                              const std::set<SgInitializedName*> readOnlyVars,
                              std::string wrapper_arg_name,
                              SgScopeStatement* scope);
  
};

#endif // !defined(INC_LIAOUTLINER_HH)

// eof
