#ifndef ROSE_SAGE_INTERFACE
#define ROSE_SAGE_INTERFACE

#include "sage3basic.hhh"
#include <stdint.h>

#if 0   // FMZ(07/07/2010): the argument "nextErrorCode" should be call-by-reference
SgFile* determineFileType ( std::vector<std::string> argv, int nextErrorCode, SgProject* project );
#else
SgFile* determineFileType ( std::vector<std::string> argv, int& nextErrorCode, SgProject* project );
#endif

#include "rewrite.h"

// DQ (7/20/2008): Added support for unparsing abitrary strings in the unparser.
#include "astUnparseAttribute.h"
#include <set>

#include "LivenessAnalysis.h"
#include "abstract_handle.h"

// DQ (8/19/2004): Moved from ROSE/src/midend/astRewriteMechanism/rewrite.h
//! A global function for getting the string associated with an enum (which is defined in global scope)
std::string getVariantName (VariantT v);

// DQ (12/9/2004): Qing, Rich and Dan have decided to start this namespace within ROSE
// This namespace is specific to interface functions that operate on the Sage III AST.
// The name was chosen so as not to conflict with other classes within ROSE.
// This will become the future home of many interface functions which operate on
// the AST and which are generally useful to users.  As a namespace multiple files can be used
// to represent the compete interface and different developers may contribute interface
// functions easily.

// Constructor handling: (We have sageBuilder.h now for this purpose, Liao 2/1/2008)
//     We could add simpler layers of support for construction of IR nodes by
// hiding many details in "makeSg***()" functions. Such functions would
// return pointers to the associated Sg*** objects and would be able to hide
// many IR specific details, including:
//      memory handling
//      optional parameter settings not often required
//      use of Sg_File_Info objects (and setting them as transformations)
//
// namespace AST_Interface  (this name is taken already by some of Qing's work :-)

//! An alias for Sg_File_Info::generateDefaultFileInfoForTransformationNode()
#define TRANS_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

//------------------------------------------------------------------------
/*! \brief This namespace is to organize functions that are useful when operating on the AST.

  \defgroup frontendSageUtilityFunctions SAGE III utility functions(SageInterface)
  \ingroup ROSE_FrontEndGroup

    The Sage III IR design attempts to be minimalist. Thus additional functionality is
intended to be presented using separate higher level interfaces which work with the IR.
The namespace, SageInterface, collects functions that operate on the IR and are supportive of numerous types of routine operations required to support general analysis and transformation of the AST.

    \internal Further organization of the functions in this namespace is required.
Major AST manipulation functions are scattered in the following directories
   - src/midend/astUtil/astInterface
   - src/roseSupport/utility_function.h,  namespace ROSE
   - src/roseSupport/TransformationSupport.h, class TransformationSupport
   - src/midend/astInlining/inlinerSupport.C
   - src/frontend/SageIII/sageInterface
   - projects: such as outliner, OpenMP_Translator
Some other utility functions not related AST can be found in
   - src/util/stringSupport/string_functions.h, namespace StringUtility
   - src/roseExtensions/dataStructureTraversal/helpFunctions.C
   - projects/dataStructureGraphing/helpFunctions.C


    \todo A number of additional things to do:
         - Pull scope handling out of EDG/Sage III translation so that is is made
           available to anyone else building the Sage III IR from scratch (which
           when it gets non-trivial, involves the manipulation of scopes).
         - Other stuff ...
 */
namespace SageInterface
{
//! An internal counter for generating unique SgName
extern int gensym_counter;

// tps : 28 Oct 2008 - support for finding the main interpretation
 SgAsmInterpretation* getMainInterpretation(SgAsmGenericFile* file);

//! Get the unsigned value of a disassembled constant.
uint64_t getAsmConstant(SgAsmValueExpression* e);

//! Get the signed value of a disassembled constant.
int64_t getAsmSignedConstant(SgAsmValueExpression *e);

//! Function to add "C" style comment to statement.
 void addMessageStatement( SgStatement* stmt, std::string message );

//! A persistent attribute to represent a unique name for an expression
  class UniqueNameAttribute : public AstAttribute
  {
    private:
     std::string name;
    public:
     UniqueNameAttribute(std::string n="") {name =n; };
     void set_name (std::string n) {name = n;};
     std::string get_name () {return name;};
  };

// DQ (3/2/2009): Added support for collectiong an merging the referenced symbols in the outlined
// function into the list used to edit the outlined code subtree to fixup references (from symbols
// in the original file to the symbols in the newer separate file).
// typedef rose_hash::unordered_map<SgNode*, SgNode*, hash_nodeptr> ReplacementMapType;
// void supplementReplacementSymbolMap ( const ReplacementMapTraversal::ReplacementMapType & inputReplacementMap );

// CH (4/9/2010): Use boost::hash instead
//#ifdef _MSC_VER
#if 0
inline size_t hash_value(SgNode* t) {return (size_t)t;}
#endif

struct hash_nodeptr
   {
// CH (4/9/2010): Use boost::hash instead
//#ifndef _MSC_VER
#if 0
           //rose_hash::hash<char*> hasher;
#endif
     public:
          size_t operator()(SgNode* node) const
             {
// CH (4/9/2010): Use boost::hash instead
//#ifdef _MSC_VER
 #if 0
                                  return (size_t) hash_value(node);
 #else
                                  return (size_t) node;
 #endif
                   }
    };

  void supplementReplacementSymbolMap ( rose_hash::unordered_map<SgNode*, SgNode*, hash_nodeptr> & inputReplacementMap );

 //------------------------------------------------------------------------
 //@{
 /*! @name Symbol tables
   \brief  utility functions for symbol tables
 */
   // Liao 1/22/2008, used for get symbols for generating variable reference nodes
   // ! Find a variable symbol in current and ancestor scopes for a given name
   SgVariableSymbol *lookupVariableSymbolInParentScopes (const SgName &	name,
                                                         SgScopeStatement *currentScope=NULL);
   //! Find a symbol in current and ancestor scopes for a given variable name, starting from top of ScopeStack if currentscope is not given or NULL.
   SgSymbol *lookupSymbolInParentScopes (const SgName &	name,
                                                         SgScopeStatement *currentScope=NULL);

   // DQ (11/24/2007): Functions moved from the Fortran support so that they could be called from within astPostProcessing.
   //!look up the first matched function symbol in parent scopes given only a function name, starting from top of ScopeStack if currentscope is not given or NULL
   SgFunctionSymbol *lookupFunctionSymbolInParentScopes (const SgName &	functionName,
                                                         SgScopeStatement *currentScope=NULL);

   // Liao, 1/24/2008, find exact match for a function
   //!look up function symbol in parent scopes given both name and function type, starting from top of ScopeStack if currentscope is not given or NULL
   SgFunctionSymbol *lookupFunctionSymbolInParentScopes (const SgName &  functionName,
                                                         const SgType* t,
                                                         SgScopeStatement *currentScope=NULL);

   /*! \brief set_name of symbol in symbol table.

       This function extracts the symbol from the relavant symbol table,
       changes the name (at the declaration) and reinserts it into the
       symbol table.

       \internal  I think this is what this function does, I need to double check.
    */
   // DQ (12/9/2004): Moved this function (by Alin Jula) from being a member of SgInitializedName
   // to this location where it can be a part of the interface for the Sage III AST.
   int set_name (SgInitializedName * initializedNameNode, SgName new_name);

   /*! \brief Output function type symbols in global function type symbol table.
    */
   void outputGlobalFunctionTypeSymbolTable ();

   // DQ (6/27/2005):
   /*! \brief Output the local symbol tables.

       \implementation Each symbol table is output with the file infor where it is located in the source code.
    */
   void outputLocalSymbolTables (SgNode * node);

   class OutputLocalSymbolTables:public AstSimpleProcessing
         {
           public:
     void visit (SgNode * node);
         };
   /*! \brief Regenerate the symbol table.

      \implementation current symbol table must be NULL pointer before calling this
      function (for safety, but is this a good idea?)
    */
   // DQ (9/28/2005):
   void rebuildSymbolTable (SgScopeStatement * scope);

   /*! \brief Clear those variable symbols (together with initialized names) which are not referenced by any variable references or declarations.
    */
   void clearUnusedVariableSymbols ();

   // DQ (3/1/2009):
   //! All the symbol table references in the copied AST need to be reset after rebuilding the copied scope's symbol table.
   void fixupReferencesToSymbols( const SgScopeStatement* this_scope,  SgScopeStatement* copy_scope, SgCopyHelp & help );

 //@}

 //------------------------------------------------------------------------
 //@{
 /*! @name Stringify
   \brief Generate a useful string (name) to describe a SgNode
 */
   /*! \brief Generate a useful name to describe the SgNode

       \internal default names are used for SgNode objects that can not be associated with a name.
    */
   // DQ (9/21/2005): General function for extracting the name of declarations (when they have names)
     std::string get_name (const SgNode * node);

   /*! \brief Generate a useful name to describe the declaration

       \internal default names are used for declarations that can not be associated with a name.
    */
   // DQ (6/13/2005): General function for extracting the name of declarations (when they have names)
     std::string get_name (const SgStatement * stmt);

   /*! \brief Generate a useful name to describe the expression

       \internal default names are used for expressions that can not be associated with a name.
    */
     std::string get_name (const SgExpression * expr);

   /*! \brief Generate a useful name to describe the declaration

       \internal default names are used for declarations that can not be associated with a name.
    */
   // DQ (6/13/2005): General function for extracting the name of declarations (when they have names)
     std::string get_name (const SgDeclarationStatement * declaration);

   /*! \brief Generate a useful name to describe the scope

       \internal default names are used for scope that cannot be associated with a name.
    */
   // DQ (6/13/2005): General function for extracting the name of declarations (when they have names)
     std::string get_name (const SgScopeStatement * scope);

   /*! \brief Generate a useful name to describe the SgSymbol

       \internal default names are used for SgSymbol objects that cannot be associated with a name.
    */
   // DQ (2/11/2007): Added this function to make debugging support more complete (useful for symbol table debugging support).
     std::string get_name (const SgSymbol * symbol);

   /*! \brief Generate a useful name to describe the SgType

       \internal default names are used for SgType objects that cannot be associated with a name.
    */
     std::string get_name (const SgType * type);


   /*! \brief Generate a useful name to describe the SgSupport IR node
    */
     std::string get_name (const SgSupport * node);


   /*! \brief Generate a useful name to describe the SgLocatedNodeSupport IR node
    */
     std::string get_name (const SgLocatedNodeSupport * node);

   /*! \brief Generate a useful name to describe the SgC_PreprocessorDirectiveStatement IR node
    */
     std::string get_name ( const SgC_PreprocessorDirectiveStatement* directive );

   /*! \brief Generate a useful name to describe the SgToken IR node
    */
     std::string get_name ( const SgToken* token );

 //@}

 //------------------------------------------------------------------------
 //@{
 /*! @name Class utilities
   \brief
 */
   /*! \brief Get the default destructor from the class declaration
    */
   // DQ (6/21/2005): Get the default destructor from the class declaration
   SgMemberFunctionDeclaration *getDefaultDestructor (SgClassDeclaration *
                                                      classDeclaration);

   /*! \brief Get the default constructor from the class declaration
    */
   // DQ (6/22/2005): Get the default constructor from the class declaration
   SgMemberFunctionDeclaration *getDefaultConstructor (SgClassDeclaration *
                                                       classDeclaration);
   /*! \brief Return true if template definition is in the class, false if outside of class.
    */
   // DQ (8/27/2005):
   bool templateDefinitionIsInClass (SgTemplateInstantiationMemberFunctionDecl
                                     * memberFunctionDeclaration);

   /*! \brief Generate a non-defining (forward) declaration from a defining function declaration.

      \internal should put into sageBuilder ?
    */
   // DQ (9/17/2005):
      SgTemplateInstantiationMemberFunctionDecl*
      buildForwardFunctionDeclaration
         (SgTemplateInstantiationMemberFunctionDecl * memberFunctionInstantiation);

   //! Check if a SgNode is a declaration for a structure
   bool isStructDeclaration(SgNode * node);
 #if 0
 // DQ (8/28/2005): This is already a member function of the SgFunctionDeclaration
 // (so that it can handle template functions and member functions)

   /*! \brief Return true if member function of a template member function,
              of false if a non-template member function in a templated class.
    */
    // DQ (8/27/2005):
   bool isTemplateMemberFunction (SgTemplateInstantiationMemberFunctionDecl *
                                  memberFunctionDeclaration);
 #endif

 //@}

 //------------------------------------------------------------------------
 //@{
 /*! @name Misc.
   \brief Not sure the classifications right now
 */

   /*! \brief Return true if function is overloaded.
    */
   // DQ (8/27/2005):
   bool isOverloaded (SgFunctionDeclaration * functionDeclaration);

  //! Generate unique names for expressions and attach the names as persistent attributes ("UniqueNameAttribute")
  void annotateExpressionsWithUniqueNames (SgProject* project);

   //! Check if a SgNode is a main() function declaration
   bool isMain (const SgNode* node);
   // DQ (6/22/2005):
   /*! \brief Generate unique name from C and C++ constructs. The name may contain space.

       This is support for the AST merge, but is generally useful as a more general mechanism than
       name mangling which is more closely ties to the generation of names to support link-time function name
       resolution.  This is more general than common name mangling in that it resolves more relevant differences
       between C and C++ declarations. (e.g. the type within the declaration: "struct { int:8; } foo;").

      \implementation current work does not support expressions.

   */
    std::string generateUniqueName ( const SgNode * node, bool ignoreDifferenceBetweenDefiningAndNondefiningDeclarations);

  // DQ (8/10/2010): Added const to first parameter.
  // DQ (3/10/2007):
  //! Generate a unique string from the source file position information
    std::string declarationPositionString (const SgDeclarationStatement * declaration);

  // DQ (1/20/2007):
  //! Added mechanism to generate project name from list of file names
    std::string generateProjectName (const SgProject * project, bool supressSuffix = false );

  //! Given a SgExpression that represents a named function (or bound member
  //! function), return the mentioned function
  SgFunctionDeclaration* getDeclarationOfNamedFunction(SgExpression* func);

  //! Get the mask expression from the header of a SgForAllStatement
  SgExpression* forallMaskExpression(SgForAllStatement* stmt);

  // DQ (10/6/2006): Added support for faster mangled name generation (caching avoids recomputation).
  /*! \brief Support for faster mangled name generation (caching avoids recomputation).

   */
  void clearMangledNameCache (SgGlobal * globalScope);
  void resetMangledNameCache (SgGlobal * globalScope);
  std::string getMangledNameFromCache (SgNode * astNode);
  std::string addMangledNameToCache (SgNode * astNode, const std::string & mangledName);

  SgDeclarationStatement * getNonInstantiatonDeclarationForClass (SgTemplateInstantiationMemberFunctionDecl * memberFunctionInstantiation);

  //! a better version for SgVariableDeclaration::set_baseTypeDefininingDeclaration(), handling all side effects automatically
  //! Used to have a struct declaration embedded into a variable declaration
  void setBaseTypeDefiningDeclaration(SgVariableDeclaration* var_decl, SgDeclarationStatement *base_decl);

  // DQ (10/14/2006): This function tests the AST to see if for a non-defining declaration, the
  // bool declarationPreceedsDefinition ( SgClassDeclaration* classNonDefiningDeclaration, SgClassDeclaration* classDefiningDeclaration );
  //! Check if a defining declaration comes before of after the non-defining declaration.
  bool declarationPreceedsDefinition (SgDeclarationStatement *nonDefiningDeclaration, SgDeclarationStatement *definingDeclaration);

  // DQ (10/19/2006): Function calls have interesting context dependent rules to determine if
  // they are output with a global qualifier or not.  Were this is true we have to avoid global
  // qualifiers, since the function's scope has not been defined.  This is an example of where
  // qualification of function names in function calls are context dependent; an interesting
  // example of where the C++ language is not friendly to source-to-source processing :-).
  bool functionCallExpressionPreceedsDeclarationWhichAssociatesScope (SgFunctionCallExp * functionCall);

 /*! \brief Compute the intersection set for two ASTs.

     This is part of a test done by the copy function to compute those IR nodes in the copy that still reference the original AST.
  */
    std::vector < SgNode * >astIntersection (SgNode * original, SgNode * copy, SgCopyHelp * help = NULL);

  //! Deep copy an arbitrary subtree
   SgNode* deepCopyNode (const SgNode* subtree);

//! A template function for deep copying a subtree. It is also  used to create deepcopy functions with specialized parameter and return types. e.g SgExpression* copyExpression(SgExpression* e);
   template <typename NodeType>
   NodeType* deepCopy (const NodeType* subtree) {
     return dynamic_cast<NodeType*>(deepCopyNode(subtree));
   }

//! Deep copy an expression
   SgExpression* copyExpression(SgExpression* e);

//!Deep copy a statement
   SgStatement* copyStatement(SgStatement* s);

// from VarSym.cc in src/midend/astOutlining/src/ASTtools
//! Get the variable symbol for the first initialized name of a declaration stmt.
  SgVariableSymbol* getFirstVarSym (SgVariableDeclaration* decl);

//! Get the first initialized name of a declaration statement
  SgInitializedName* getFirstInitializedName (SgVariableDeclaration* decl);

//! A special purpose statement removal function, originally from inlinerSupport.h, Need Jeremiah's attention to refine it. Please don't use it for now.
void myRemoveStatement(SgStatement* stmt);

bool isConstantTrue(SgExpression* e);
bool isConstantFalse(SgExpression* e);

bool isCallToParticularFunction(SgFunctionDeclaration* decl, SgExpression* e);
bool isCallToParticularFunction(const std::string& qualifiedName, size_t arity, SgExpression* e);

//! Check if a declaration has a "static' modifier
bool isStatic(SgDeclarationStatement* stmt);

//! Set a declaration as static
void setStatic(SgDeclarationStatement* stmt);

//! Check if a declaration has an "extern" modifier
bool isExtern(SgDeclarationStatement* stmt);

//! Set a declaration as extern
void setExtern(SgDeclarationStatement* stmt);

//! Interface for creating a statement whose computation writes its answer into
//! a given variable.
class StatementGenerator {
  public:
  virtual ~StatementGenerator() {};
  virtual SgStatement* generate(SgExpression* where_to_write_answer) = 0;
};

//! Check if a SgNode _s is an assignment statement (any of =,+=,-=,&=,/=, ^=, etc)
//!
//! Return the left hand, right hand expressions and if the left hand variable is also being read
  bool isAssignmentStatement(SgNode* _s, SgExpression** lhs=NULL, SgExpression** rhs=NULL, bool* readlhs=NULL);

//! Variable references can be introduced by SgVarRef, SgPntrArrRefExp, SgInitializedName, SgMemberFunctionRef etc. This function will convert them all to  a top level SgInitializedName.
SgInitializedName* convertRefToInitializedName(SgNode* current);

//! Obtain a matching SgNode from an abstract handle string
SgNode* getSgNodeFromAbstractHandleString(const std::string& input_string);

//! Dump information about a SgNode for debugging
void dumpInfo(SgNode* node, std::string desc="");

//! Reorder a list of declaration statements based on their appearance order in source files
std::vector<SgDeclarationStatement*>
sortSgNodeListBasedOnAppearanceOrderInSource(const std::vector<SgDeclarationStatement*>& nodevec);

//@}

//------------------------------------------------------------------------
//@{
/*! @name AST properties
  \brief version, language properties of current AST.
*/

//  std::string version();  // utility_functions.h, version number
  /*! Brief These traverse the memory pool of SgFile IR nodes and determine what languages are in use!
   */
  bool is_C_language ();
  bool is_OpenMP_language ();
  bool is_UPC_language ();
  //! Check if dynamic threads compilation is used for UPC programs
  bool is_UPC_dynamic_threads();
  bool is_C99_language ();
  bool is_Cxx_language ();
  bool is_Java_language ();
  bool is_Fortran_language ();
  bool is_CAF_language ();
  bool is_PHP_language();
  bool is_binary_executable();
  bool is_mixed_C_and_Cxx_language ();
  bool is_mixed_Fortran_and_C_language ();
  bool is_mixed_Fortran_and_Cxx_language ();
  bool is_mixed_Fortran_and_C_and_Cxx_language ();
//@}

//------------------------------------------------------------------------
//@{
/*! @name Scope
  \brief
*/

  // DQ (10/5/2006): Added support for faster (non-quadratic) computation of unique
  // labels for scopes in a function (as required for name mangling).
  /*! \brief Assigns unique numbers to each SgScopeStatement of a function.

      This is used to provide unique names for variables and types defined is
      different nested scopes of a function (used in mangled name generation).
   */
  void resetScopeNumbers (SgFunctionDefinition * functionDeclaration);

  // DQ (10/5/2006): Added support for faster (non-quadratic) computation of unique
  // labels for scopes in a function (as required for name mangling).
  /*! \brief Clears the cache of scope,integer pairs for the input function.

      This is used to clear the cache of computed unique lables for scopes in a function.
      This function should be called after any transformation on a function that might effect
      the allocation of scopes and cause the existing unique numbrs to be incorrect.
      This is part of support to provide unique names for variables and types defined is
      different nested scopes of a function (used in mangled name generation).
   */
  void clearScopeNumbers (SgFunctionDefinition * functionDefinition);


  //!Find the enclosing namespace of a declaration
  SgNamespaceDefinitionStatement * enclosingNamespaceScope (SgDeclarationStatement * declaration);
//  SgNamespaceDefinitionStatement * getEnclosingNamespaceScope (SgNode * node);

  bool isPrototypeInScope (SgScopeStatement * scope,
                           SgFunctionDeclaration * functionDeclaration,
			   SgDeclarationStatement * startingAtDeclaration);

  //!check if node1 is a strict ancestor of node 2. (a node is not considered its own ancestor)
  bool isAncestor(SgNode* node1, SgNode* node2);
//@}
//------------------------------------------------------------------------
//@{
/*! @name Preprocessing Information
  \brief #if-#else-#end, comments, #include, etc
*/

  //! Dumps a located node's preprocessing information.
  void dumpPreprocInfo (SgLocatedNode* locatedNode);

//! Insert  #include "filename" or #include <filename> (system header) into the global scope containing the current scope, right after other #include XXX.
PreprocessingInfo* insertHeader(const std::string& filename, PreprocessingInfo::RelativePositionType position=PreprocessingInfo::after, bool isSystemHeader=false, SgScopeStatement* scope=NULL);

//! Identical to movePreprocessingInfo(), except for the stale name and confusing order of parameters. It will be deprecated soon.
void moveUpPreprocessingInfo (SgStatement* stmt_dst, SgStatement* stmt_src, PreprocessingInfo::RelativePositionType src_position=PreprocessingInfo::undef,  PreprocessingInfo::RelativePositionType dst_position=PreprocessingInfo::undef, bool usePrepend= false);

//! Move preprocessing information of stmt_src to stmt_dst, Only move preprocessing information from the specified source-relative position to a specified target position, otherwise move all preprocessing information with position information intact. The preprocessing information is appended to the existing preprocessing information list of the target node by default. Prepending is used if usePreprend is set to true. Optionally, the relative position can be adjust after the moving using dst_position.
void movePreprocessingInfo (SgStatement* stmt_src, SgStatement* stmt_dst,  PreprocessingInfo::RelativePositionType src_position=PreprocessingInfo::undef,
                             PreprocessingInfo::RelativePositionType dst_position=PreprocessingInfo::undef, bool usePrepend= false);


//!Cut preprocessing information from a source node and save it into a buffer. Used in combination of pastePreprocessingInfo(). The cut-paste operation is similar to moveUpPreprocessingInfo() but it is more flexible in that the destination node can be unknown during the cut operation.
void cutPreprocessingInfo (SgLocatedNode* src_node, PreprocessingInfo::RelativePositionType pos, AttachedPreprocessingInfoType& save_buf);

//!Paste preprocessing information from a buffer to a destination node. Used in combination of cutPreprocessingInfo()
void pastePreprocessingInfo (SgLocatedNode* dst_node, PreprocessingInfo::RelativePositionType pos, AttachedPreprocessingInfoType& saved_buf);

//! Attach an arbitrary string to a located node. A workaround to insert irregular statements or vendor-specific attributes.
PreprocessingInfo* attachArbitraryText(SgLocatedNode* target,
                const std::string & text,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before);

//!Check if a pragma declaration node has macro calls attached, if yes, replace macro calls within the pragma string with expanded strings. This only works if -rose:wave is turned on.
void replaceMacroCallsWithExpandedStrings(SgPragmaDeclaration* target);
//@}


//------------------------------------------------------------------------
//@{
/*! @name Source File Position
  \brief set Sg_File_Info for a SgNode
*/
//! Build and attach comment, comment style is inferred from the language type of the target node if not provided
   PreprocessingInfo* attachComment(SgLocatedNode* target, const std::string & content,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before,
               PreprocessingInfo::DirectiveType dtype= PreprocessingInfo::CpreprocessorUnknownDeclaration);

// DQ (11/25/2009): Added matching support for adding comments to SgAsm nodes.
// Build and attach comment
// void attachComment(SgAsmStatement* target, const std::string & content );

// DQ (7/20/2008): I am not clear were I should put this function, candidates include: SgLocatedNode or SgInterface
//! Add a string to be unparsed to support code generation for back-end specific tools or compilers.
  void addTextForUnparser ( SgNode* astNode, std::string s, AstUnparseAttribute::RelativePositionType inputlocation );


  // Liao, 1/8/2007, set file info. for a whole subtree as transformation generated
//! Set current node's source position as transformation generated
  void setOneSourcePositionForTransformation(SgNode *node);

//! Set current node's source position as NULL
  void setOneSourcePositionNull(SgNode *node);

//! Recursively set source position info(Sg_File_Info) as transformation generated
  void setSourcePositionForTransformation (SgNode * root);

//! Set source position info(Sg_File_Info) as transformation generated for all SgNodes in memory pool
  void setSourcePositionForTransformation_memoryPool();

//! Set the source position of SgLocatedNode to Sg_File_Info::generateDefaultFileInfo(). These nodes WILL be unparsed. Not for transformation usage.
  void setSourcePosition (SgLocatedNode * locatedNode);

//@}


//------------------------------------------------------------------------
//@{
/*! @name Data types
  \brief
*/

// from src/midend/astInlining/typeTraits.h
// src/midend/astUtil/astInterface/AstInterface.h

//! Get the right bool type according to C or C++ language input
SgType* getBoolType(SgNode* n);


//! Check if a type is an integral type, only allowing signed/unsigned short, int, long, long long.
////!
////! There is another similar function named SgType::isIntegerType(), which allows additional types char, wchar, and bool to be treated as integer types
bool isStrictIntegerType(SgType* t);
//!Get the data type of the first initialized name of a declaration statement
SgType* getFirstVarType(SgVariableDeclaration* decl);

//! Is a type default constructible?  This may not quite work properly.
bool isDefaultConstructible(SgType* type);

//! Is a type copy constructible?  This may not quite work properly.
bool isCopyConstructible(SgType* type);

//! Is a type assignable?  This may not quite work properly.
bool isAssignable(SgType* type);

//! Does a type have a trivial (built-in) destructor?
bool hasTrivialDestructor(SgType* t);

//! Is this type a non-constant reference type? (Handles typedefs correctly)
bool isNonconstReference(SgType* t);

//! Is this type a const or non-const reference type? (Handles typedefs correctly)
bool isReferenceType(SgType* t);

//! Is this type a pointer type? (Handles typedefs correctly)
bool isPointerType(SgType* t);

//! Is this a const type?
/* const char* p = "aa"; is not treated as having a const type. It is a pointer to const char.
 * Similarly, neither for const int b[10]; or const int & c =10;
 * The standard says, "A compound type is not cv-qualified by the cv-qualifiers (if any) of
the types from which it is compounded. Any cv-qualifiers applied to an array type affect the array element type, not the array type".
 */
bool isConstType(SgType* t);

//! Remove const (if present) from a type.  stripType() cannot do this because it removes all modifiers.
SgType* removeConst(SgType* t);

//! Is this a volatile type?
bool isVolatileType(SgType* t);

//! Is this a restrict type?
bool isRestrictType(SgType* t);

//! Is this a scalar type?
/*! We define the following SgType as scalar types: char, short, int, long , void, Wchar, Float, double, long long, string, bool, complex, imaginary
 */
bool isScalarType(SgType* t);

//! Check if a type is an integral type, only allowing signed/unsigned short, int, long, long long.
//!
//! There is another similar function named SgType::isIntegerType(), which allows additional types char, wchar, and bool.
bool isStrictIntegerType(SgType* t);

//! Check if a type is a struct type (a special SgClassType in ROSE)
bool isStructType(SgType* t);

//! Generate a mangled string for a given type based on Itanium C++ ABI
std::string mangleType(SgType* type);

//! Generate mangled scalar type names according to Itanium C++ ABI, the input type should pass isScalarType() in ROSE
  std::string mangleScalarType(SgType* type);

//! Generated mangled modifier types, include const, volatile,according to Itanium C++ ABI, with extension to handle UPC shared types.
  std::string mangleModifierType(SgModifierType* type);

//! Calculate the number of elements of an array type: dim1* dim2*... , assume element count is 1 for int a[]; Strip off THREADS if it is a UPC array.
size_t getArrayElementCount(SgArrayType* t);

//! Get the number of dimensions of an array type
int getDimensionCount(SgType* t);

//! Get the element type of an array
SgType* getArrayElementType(SgType* t);

//! Get the element type of an array, pointer or string, or NULL if not applicable
SgType* getElementType(SgType* t);

//! Check if an expression is an array access. If so, return its name expression and subscripts if requested. Based on AstInterface::IsArrayAccess()
bool isArrayReference(SgExpression* ref, SgExpression** arrayNameExp=NULL, std::vector<SgExpression*>** subscripts=NULL);


//! Has a UPC shared type of any kinds (shared-to-shared, private-to-shared, shared-to-private, shared scalar/array)? An optional parameter, mod_type_out, stores the first SgModifierType with UPC access information.
/*!
 * Note: we classify private-to-shared as 'has shared' type for convenience here. It is indeed a private type in strict sense.
  AST graph for some examples:
    - shared scalar: SgModifierType -->base type
    - shared array: SgArrayType --> SgModiferType --> base type
    - shared to shared: SgModifierType --> SgPointerType --> SgModifierType ->SgTypeInt
    - shared to private: SgModifierType --> SgPointerType --> base type
    - private to shared: SgPointerType --> SgModifierType --> base type
 */
bool hasUpcSharedType(SgType* t, SgModifierType ** mod_type_out = NULL  );

//! Check if a type is a UPC shared type, including shared array, shared pointers etc. Exclude private pointers to shared types. Optionally return the modifier type with the UPC shared property.
/*!
 * ROSE uses SgArrayType of SgModifierType to represent shared arrays, not SgModifierType points to SgArrayType. Also typedef may cause a chain of nodes before reach the actual SgModifierType with UPC shared property.
 */
bool isUpcSharedType(SgType* t, SgModifierType ** mod_type_out = NULL);

//! Check if a modifier type is a UPC shared type.
bool isUpcSharedModifierType (SgModifierType* mod_type);

//! Check if an array type is a UPC shared type. ROSE AST represents a UPC shared array as regular array of elements of UPC shared Modifier Type. Not directly a UPC shared Modifier Type of an array.
bool isUpcSharedArrayType (SgArrayType* array_type);

//! Check if a shared UPC type is strict memory consistency or not. Return false if it is relaxed. (So isUpcRelaxedSharedModifierType() is not necessary.)
bool isUpcStrictSharedModifierType(SgModifierType* mode_type);

//! Get the block size of a UPC shared modifier type
size_t getUpcSharedBlockSize(SgModifierType* mod_type);

//! Get the block size of a UPC shared type, including Modifier types and array of modifier types (shared arrays)
size_t getUpcSharedBlockSize(SgType* t);

//! Is UPC phase-less shared type? Phase-less means block size of the first SgModifierType with UPC information is 1 or 0/unspecified. Also return false if the type is not a UPC shared type.
bool isUpcPhaseLessSharedType (SgType* t);

//! Is a UPC private-to-shared pointer?  SgPointerType comes first compared to SgModifierType with UPC information. Input type must be any of UPC shared types first.
bool isUpcPrivateToSharedType(SgType* t);

//! Is a UPC array with dimension of X*THREADS
bool isUpcArrayWithThreads(SgArrayType* t);

//! Lookup a named type based on its name, bottomup searching from a specified scope. Note name collison might be allowed for c (not C++) between typedef and enum/struct. Only the first matched named type will be returned in this case. typedef is returned as it is, not the base type it actually refers to.
SgType* lookupNamedTypeInParentScopes(const std::string& type_name, SgScopeStatement* scope=NULL);

//@}

//------------------------------------------------------------------------
//@{
/*! @name Loop handling
  \brief
*/

// by Jeremiah
//! Add a step statement to the end of a loop body
//! Add a new label to the end of the loop, with the step statement after
//! it; then change all continue statements in the old loop body into
//! jumps to the label
//!
//! For example:
//! while (a < 5) {if (a < -3) continue;} (adding "a++" to end) becomes
//! while (a < 5) {if (a < -3) goto label; label: a++;}
void addStepToLoopBody(SgScopeStatement* loopStmt, SgStatement* step);

void moveForStatementIncrementIntoBody(SgForStatement* f);
void convertForToWhile(SgForStatement* f);
void convertAllForsToWhiles(SgNode* top);
//! Change continue statements in a given block of code to gotos to a label
void changeContinuesToGotos(SgStatement* stmt, SgLabelStatement* label);

//!Return the loop index variable for a for loop
SgInitializedName* getLoopIndexVariable(SgNode* loop);

//! Routines to get and set the body of a loop
SgStatement* getLoopBody(SgScopeStatement* loop);

void setLoopBody(SgScopeStatement* loop, SgStatement* body);

//! Routines to get the condition of a loop. It recognize While-loop, For-loop, and Do-While-loop
SgStatement* getLoopCondition(SgScopeStatement* loop);

//! Set the condition statement of a loop, including While-loop, For-loop, and Do-While-loop.
void setLoopCondition(SgScopeStatement* loop, SgStatement* cond);

//! Check if a for-loop has a canonical form, return loop index, bounds, step, and body if requested
//!
//! A canonical form is defined as : one initialization statement, a test expression, and an increment expression , loop index variable should be of an integer type.  IsInclusiveUpperBound is true when <= or >= is used for loop condition
bool isCanonicalForLoop(SgNode* loop, SgInitializedName** ivar=NULL, SgExpression** lb=NULL, SgExpression** ub=NULL, SgExpression** step=NULL, SgStatement** body=NULL, bool *hasIncrementalIterationSpace = NULL, bool* isInclusiveUpperBound = NULL);

//! Check if a Fortran Do loop has a complete canonical form: Do I=1, 10, 1
bool isCanonicalDoLoop(SgFortranDo* loop,SgInitializedName** ivar/*=NULL*/, SgExpression** lb/*=NULL*/, SgExpression** ub/*=NULL*/, SgExpression** step/*=NULL*/, SgStatement** body/*=NULL*/, bool *hasIncrementalIterationSpace/*= NULL*/, bool* isInclusiveUpperBound/*=NULL*/);

//! Set the lower bound of a loop header for (i=lb; ...)
void setLoopLowerBound(SgNode* loop, SgExpression* lb);

//! Set the upper bound of a loop header,regardless the condition expression type.  for (i=lb; i op up, ...)
void setLoopUpperBound(SgNode* loop, SgExpression* ub);

//! Set the stride(step) of a loop 's incremental expression, regardless the expression types (i+=s; i= i+s, etc)
void setLoopStride(SgNode* loop, SgExpression* stride);


//! Normalize loop init stmt by promoting the single variable declaration statement outside of the for loop header's init statement, e.g. for (int i=0;) becomes int i_x; for (i_x=0;..) and rewrite the loop with the new index variable, if necessary
bool normalizeForLoopInitDeclaration(SgForStatement* loop);

//! Normalize a for loop, return true if successful
//!
//! Translations are :
//!    For the init statement: for (int i=0;... ) becomes int i; for (i=0;..)   
//!    For test expression:
//!           i<x is normalized to i<= (x-1) and
//!           i>x is normalized to i>= (x+1)
//!    For increment expression:
//!           i++ is normalized to i+=1 and
//!           i-- is normalized to i+=-1
//!           i-=s is normalized to i+= -s
bool forLoopNormalization(SgForStatement* loop);

//!Normalize a Fortran Do loop. Make the default increment expression (1) explicit
bool doLoopNormalization(SgFortranDo* loop);

//!  Unroll a target loop with a specified unrolling factor. It handles steps larger than 1 and adds a fringe loop if the iteration count is not evenly divisible by the unrolling factor.
bool loopUnrolling(SgForStatement* loop, size_t unrolling_factor);

//! Interchange/permutate a n-level perfectly-nested loop rooted at 'loop' using a lexicographical order number within (0,depth!).
bool loopInterchange(SgForStatement* loop, size_t depth, size_t lexicoOrder);

//! Tile the n-level (starting from 1) loop of a perfectly nested loop nest using tiling size s
bool loopTiling(SgForStatement* loopNest, size_t targetLevel, size_t tileSize);

//@}

//------------------------------------------------------------------------
//@{
/*! @name Topdown search
  \brief Top-down traversal from current node to find a node of a specified type
*/

//! Query a subtree to get all nodes of a given type, with an appropriate downcast.
template <typename NodeType>
std::vector<NodeType*> querySubTree(SgNode* top, VariantT variant = (VariantT)NodeType::static_variant)
{
  Rose_STL_Container<SgNode*> nodes = NodeQuery::querySubTree(top,variant);
  std::vector<NodeType*> result(nodes.size(), NULL);
  int count = 0;
  for (Rose_STL_Container<SgNode*>::const_iterator i = nodes.begin();
       i != nodes.end(); ++i, ++count) {
    NodeType* node = dynamic_cast<NodeType*>(*i);
    ROSE_ASSERT (node);
    result[count] = node;
  }
  return result;
}
  /*! \brief Returns STL vector of SgFile IR node pointers.

      Demonstrates use of restricted traversal over just SgFile IR nodes.
   */
    std::vector < SgFile * >generateFileList ();

  //! Get the current SgProject IR Node
  SgProject * getProject();

//! Query memory pools to grab SgNode of a specified type
template <typename NodeType>
static std::vector<NodeType*> getSgNodeListFromMemoryPool()
{
  // This function uses a memory pool traversal specific to the SgFile IR nodes
  class MyTraversal : public ROSE_VisitTraversal
  {
    public:
      std::vector<NodeType*> resultlist;
      void visit ( SgNode* node)
      {
        NodeType* result = dynamic_cast<NodeType* > (node);
        ROSE_ASSERT(result!= NULL);
        if (result!= NULL)
        {
          resultlist.push_back(result);
        }
      };
      virtual ~MyTraversal() {}
  };

  MyTraversal my_traversal;
  NodeType::visitRepresentativeNode(my_traversal);
  return my_traversal.resultlist;
}


/*! \brief top-down traversal from current node to find the main() function declaration
*/
SgFunctionDeclaration* findMain(SgNode* currentNode);

//! find the last declaration statement within a scope (if any). This is often useful to decide where to insert another declaration statement
SgStatement* findLastDeclarationStatement(SgScopeStatement * scope);

	  //midend/programTransformation/partialRedundancyElimination/pre.h
//! find referenced symbols within an expression
std::vector<SgVariableSymbol*> getSymbolsUsedInExpression(SgExpression* expr);

//! Find break statements inside a particular statement, stopping at nested loops or switches
/*! loops or switch statements defines their own contexts for break
 statements.  The function will stop immediately if run on a loop or switch
 statement.  If fortranLabel is non-empty, breaks (EXITs) to that label within
 nested loops are included in the returned list.
*/
std::vector<SgBreakStmt*> findBreakStmts(SgStatement* code, const std::string& fortranLabel = "");

//! Find all continue statements inside a particular statement, stopping at nested loops
/*! Nested loops define their own contexts for continue statements.  The
 function will stop immediately if run on a loop
 statement.  If fortranLabel is non-empty, continues (CYCLEs) to that label
 within nested loops are included in the returned list.
*/
  std::vector<SgContinueStmt*> findContinueStmts(SgStatement* code, const std::string& fortranLabel = "");
  std::vector<SgGotoStatement*> findGotoStmts(SgStatement* scope, SgLabelStatement* l);
  std::vector<SgStatement*> getSwitchCases(SgSwitchStatement* sw);

  //! Find a declaration given its name, scope, and defining or nondefining flag.
  template <typename T>
  T* findDeclarationStatement(SgNode* root, std::string name, SgScopeStatement* scope, bool isDefining)
  {
    if (!root) return 0;
    T* decl = dynamic_cast<T*>(root);
    if (decl!=NULL)
    {
     if ((decl->get_scope() == scope)&&
       (decl->search_for_symbol_from_symbol_table()->get_name()==name))
     return decl;
    }

    std::vector<SgNode*> children = root->get_traversalSuccessorContainer();
    for (std::vector<SgNode*>::const_iterator i = children.begin();
            i != children.end(); ++i)
    {
     T* target= findDeclarationStatement<T> (*i,name, scope, isDefining);
     if (target)
       return target;
    }
    return 0;
  }

#if 0 //TODO
  // 1. preorder traversal from current SgNode till find next SgNode of type V_SgXXX
  //    until reach the end node
  SgNode* getNextSgNode( const SgNode* astSourceNode, VariantT=V_SgNode, SgNode* astEndNode=NULL);

  // 2. return all nodes of type VariantT following the source node
  std::vector<SgNode*> getAllNextSgNode( const SgNode* astSourceNode, VariantT=V_SgNode, SgNode* astEndNode=NULL);

#endif

//@}

//------------------------------------------------------------------------
//@{
/*! @name Bottom up search
  \brief Backwards traverse through the AST to find a node, findEnclosingXXX()
*/
// remember to put const to all arguments.
//! Traverse back through a node's parents to find the first node matching the desired type and its derived types, includingSelf specifies if the current node is checked.
template <typename NodeType>
NodeType* getEnclosingNode(const SgNode* astNode, const bool includingSelf=false)
{
  ROSE_ASSERT(astNode!=NULL);
  if ((includingSelf)&&(dynamic_cast<const NodeType*>(astNode)))
    return const_cast<NodeType*>(dynamic_cast<const NodeType*> (astNode));

  SgNode* parent = astNode->get_parent();
  while ((parent!=NULL)&&(!dynamic_cast<const NodeType*>(parent)))
    parent = parent->get_parent();

  return const_cast<NodeType*>(dynamic_cast<const NodeType*> (parent));
}

//! Get the closest scope
SgScopeStatement* getScope(const SgNode* astNode);

  //! Traverse back through a node's parents to find the enclosing global scope
  SgGlobal* getGlobalScope( const SgNode* astNode);


  //! Find the function definition
  SgFunctionDefinition* getEnclosingProcedure(SgNode* n, const bool includingSelf=false);

  SgFunctionDefinition* getEnclosingFunctionDefinition(SgNode* astNode, const bool includingSelf=false);

  //! Find the closest enclosing statement, including the given node
  SgStatement* getEnclosingStatement(SgNode* n);

  //! Find the closest switch outside a given statement (normally used for case and default statements)
  SgSwitchStatement* findEnclosingSwitch(SgStatement* s);

  //! Find the closest loop outside the given statement; if fortranLabel is not empty, the Fortran label of the loop must be equal to it
  SgScopeStatement* findEnclosingLoop(SgStatement* s, const std::string& fortranLabel = "", bool stopOnSwitches = false);

  //! Find the enclosing function declaration, including its derived instances like isSgProcedureHeaderStatement, isSgProgramHeaderStatement, and isSgMemberFunctionDeclaration.
  SgFunctionDeclaration * getEnclosingFunctionDeclaration (SgNode * astNode, const bool includingSelf=false);
   //roseSupport/utility_functions.h
  //! get the SgFile node from current node
  SgFile* getEnclosingFileNode (SgNode* astNode );

  //! Get the initializer containing an expression if it is within an initializer.
  SgInitializer* getInitializerOfExpression(SgExpression* n);

  //! Get the closest class definition enclosing the specified AST node,
  SgClassDefinition* getEnclosingClassDefinition(SgNode* astnode, const bool includingSelf=false);

// TODO
#if 0
   SgNode * getEnclosingSgNode(SgNode* source,VariantT, SgNode* endNode=NULL);
   std::vector<SgNode *>  getAllEnclosingSgNode(SgNode* source,VariantT, SgNode* endNode=NULL);
   SgVariableDeclaration* findVariableDeclaratin( const string& varname)

   SgClassDeclaration* getEnclosingClassDeclaration( const SgNode* astNode);

   // e.g. for some expression, find its parent statement
   SgStatement* getEnclosingStatement(const SgNode* astNode);

   SgSwitchStatement* getEnclosingSwitch(SgStatement* s);
   SgModuleStatement* getEnclosingModuleStatement( const SgNode* astNode);

  // used to build a variable reference for compiler generated code in current scope
   SgSymbol * findReachingDefinition (SgScopeStatement* startScope, SgName &name);
#endif
//@}

//------------------------------------------------------------------------
//@{
/*! @name AST Walk and Traversal
  \brief
*/
  // Liao, 1/9/2008
  /*!
  	\brief return the first global scope under current project
  */
  SgGlobal * getFirstGlobalScope(SgProject *project);

  /*!
	\brief get the last statement within a scope, return NULL if it does not exit
  */
  SgStatement* getLastStatement(SgScopeStatement *scope);

  //! Get the first statement within a scope, return NULL if it does not exist. Skip compiler-generated statement by default. Count transformation-generated ones, but excluding those which are not to be outputted in unparsers.
  SgStatement* getFirstStatement(SgScopeStatement *scope,bool includingCompilerGenerated=false);
    //!Find the first defining function declaration statement in a scope
  SgFunctionDeclaration* findFirstDefiningFunctionDecl(SgScopeStatement* scope);

//! Get next statement within the same scope of current statement
  SgStatement* getNextStatement(SgStatement * currentStmt);

//! Get previous statement within the same scope of current statement
  SgStatement* getPreviousStatement(SgStatement * currentStmt);
#if 0 //TODO
  // preorder traversal from current SgNode till find next SgNode of type V_SgXXX
  SgNode* getNextSgNode( const SgNode* currentNode, VariantT=V_SgNode);
#endif
//@}

//------------------------------------------------------------------------
//@{
/*! @name AST Comparison
  \brief Compare AST nodes, subtree, etc
*/
  //! Check if a SgIntVal node has a given value
 bool isEqualToIntConst(SgExpression* e, int value);

 //! Check if two function declarations refer to the same one. Two function declarations are the same when they are a) identical, b) same name in C c) same qualified named and mangled name in C++. A nondefining (prototype) declaration and a defining declaration of a same function are treated as the same.
 /*!
  * There is a similar function bool compareFunctionDeclarations(SgFunctionDeclaration *f1, SgFunctionDeclaration *f2) from Classhierarchy.C
  */
 bool isSameFunction(SgFunctionDeclaration* func1, SgFunctionDeclaration* func2);

 //! Check if a statement is the last statement within its closed scope
 bool isLastStatement(SgStatement* stmt);

//@}

//------------------------------------------------------------------------
//@{
/*! @name AST insert, removal, and replacement
  \brief Add, remove,and replace AST

  scope->append_statement(), exprListExp->append_expression() etc. are not enough to handle side effect of parent pointers, symbol tables, preprocessing info, defining/nondefining pointers etc.
*/

// DQ (2/24/2009): Simple function to delete an AST subtree (used in outlining).
//! Function to delete AST subtree's nodes only, users must take care of any dangling pointers, symbols or types that result.
 void deleteAST(SgNode* node);

// DQ (2/25/2009): Added new function to support outliner.
//! Move statements in first block to the second block (preserves order and rebuilds the symbol table).
 void moveStatementsBetweenBlocks ( SgBasicBlock* sourceBlock, SgBasicBlock* targetBlock );


//! Append a statement to the end of the current scope, handle side effect of appending statements, e.g. preprocessing info, defining/nondefining pointers etc.
void appendStatement(SgStatement *stmt, SgScopeStatement* scope=NULL);

//! Append a list of statements to the end of the current scope, handle side effect of appending statements, e.g. preprocessing info, defining/nondefining pointers etc.
void appendStatementList(const std::vector<SgStatement*>& stmt, SgScopeStatement* scope=NULL);

// DQ (2/6/2009): Added function to support outlining into separate file.
//! Append a copy ('decl') of a function ('original_statement') into a 'scope', include any referenced declarations required if the scope is within a compiler generated file. All referenced declarations, including those from headers, are inserted if excludeHeaderFiles is set to true (the new file will not have any headers).
void appendStatementWithDependentDeclaration( SgDeclarationStatement* decl, SgGlobal* scope, SgStatement* original_statement, bool excludeHeaderFiles );

//! Prepend a statement to the beginning of the current scope, handling side
//! effects as appropriate
void prependStatement(SgStatement *stmt, SgScopeStatement* scope=NULL);

//! prepend a list of statements to the beginning of the current scope,
//! handling side effects as appropriate
void prependStatementList(const std::vector<SgStatement*>& stmt, SgScopeStatement* scope=NULL);

//! Check if a scope statement has a simple children statement list
//! so insert additional statements under the scope is straightforward and unambiguous .
//! for example, SgBasicBlock has a simple statement list while IfStmt does not.
bool  hasSimpleChildrenList (SgScopeStatement* scope);

//! Insert a statement before or after the target statement within the target's scope. Move around preprocessing info automatically
void insertStatement(SgStatement *targetStmt, SgStatement* newStmt, bool insertBefore= true, bool autoMovePreprocessingInfo = true);

//! Insert a list of statements before or after the target statement within the
//target's scope
void insertStatementList(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmts, bool insertBefore= true);

//! Insert a statement before a target statement
void insertStatementBefore(SgStatement *targetStmt, SgStatement* newStmt, bool autoMovePreprocessingInfo = true);

//! Insert a list of statements before a target statement
void insertStatementListBefore(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmts);

//! Insert a statement after a target statement, Move around preprocessing info automatically by default
void insertStatementAfter(SgStatement *targetStmt, SgStatement* newStmt, bool autoMovePreprocessingInfo = true);

//! Insert a list of statements after a target statement
void insertStatementListAfter(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmt);

//! Remove a statement from its attach point of the AST. Automatically keep its associated preprocessing information at the original place after the removal. The statement is still in memory and it is up to the users to decide if the removed one will be inserted somewhere else or released from memory (deleteAST()).
void removeStatement(SgStatement* stmt, bool autoRelocatePreprocessingInfo = true);

//! Deep delete a sub AST tree. It uses postorder traversal to delete each child node. Users must take care of any dangling pointers, symbols or types that result. This is identical to deleteAST()
void deepDelete(SgNode* root);

//! Replace a statement with another. Move preprocessing information from oldStmt to newStmt if requested.
void replaceStatement(SgStatement* oldStmt, SgStatement* newStmt, bool movePreprocessinInfo = false);

//! Append an argument to SgFunctionParameterList, transparently set parent,scope, and symbols for arguments when possible
/*! We recommend to build SgFunctionParameterList before building a function declaration
 However, it is still allowed to append new arguments for existing function declarations.
 \todo function type , function symbol also need attention.
*/
SgVariableSymbol* appendArg(SgFunctionParameterList *, SgInitializedName*);
//!Prepend an argument to SgFunctionParameterList
SgVariableSymbol* prependArg(SgFunctionParameterList *, SgInitializedName*);

//! Append an expression to a SgExprListExp, set the parent pointer also
void appendExpression(SgExprListExp *, SgExpression*);

//! Append an expression list to a SgExprListExp, set the parent pointers also
void appendExpressionList(SgExprListExp *, const std::vector<SgExpression*>&);

//! Set parameter list for a function declaration, considering existing parameter list etc.
void setParameterList(SgFunctionDeclaration *func,SgFunctionParameterList *paralist);

//! Set a pragma of a pragma declaration. handle memory release for preexisting pragma, and set parent pointer.
void setPragma(SgPragmaDeclaration* decl, SgPragma *pragma);

  //! Replace an expression with another, used for variable reference substitution and others. the old expression can be deleted (default case)  or kept.
void replaceExpression(SgExpression* oldExp, SgExpression* newExp, bool keepOldExp=false);

//! Replace a given expression with a list of statements produced by a generator
void replaceExpressionWithStatement(SgExpression* from,
				    SageInterface::StatementGenerator* to);
//! Similar to replaceExpressionWithStatement, but with more restrictions.
//! Assumptions: from is not within the test of a loop or ifStmt,  not currently traversing from or the statement it is in
void replaceSubexpressionWithStatement(SgExpression* from,
				      SageInterface::StatementGenerator* to);

//! Set operands for expressions with single operand, such as unary expressions. handle file info, lvalue, pointer downcasting, parent pointer etc.
void setOperand(SgExpression* target, SgExpression* operand);

//!set left hand operand for binary expressions, transparently downcasting target expressions when necessary
void setLhsOperand(SgExpression* target, SgExpression* lhs);

//!set left hand operand for binary expression
void setRhsOperand(SgExpression* target, SgExpression* rhs);

//! Set original expression trees to NULL for SgValueExp or SgCastExp expressions, so you can change the value and have it unparsed correctly.
void removeAllOriginalExpressionTrees(SgNode* top);

// DQ (1/25/2010): Added support for directories
//! Move file to be generated in a subdirectory (will be generated by the unparser).
void moveToSubdirectory ( std::string directoryName, SgFile* file );

//! Supporting function to comment relocation in insertStatement() and removeStatement().
SgStatement* findSurroundingStatementFromSameFile(SgStatement* targetStmt, bool & surroundingStatementPreceedsTargetStatement);

//! Relocate comments and CPP directives from one statement to another.
   void moveCommentsToNewStatement(SgStatement* sourceStatement, const std::vector<int> & indexList, SgStatement* targetStatement, bool surroundingStatementPreceedsTargetStatement);

//@}
//------------------------------------------------------------------------
//@{
/*! @name AST repair, fix, and postprocessing.
  \brief Mostly used internally when some AST pieces are built without knowing their target
  scope/parent, especially during bottom-up construction of AST. The associated symbols,
   parent and scope  pointers cannot be set on construction then.
   A set of utility functions are provided to
   patch up scope, parent, symbol for them when the target scope/parent become know.
*/
//! Connect variable reference to the right variable symbols when feasible, return the number of references being fixed.
/*! In AST translation, it is possible to build a variable reference before the variable
 is being declared. buildVarRefExp() will use fake initialized name and symbol as placeholders
 to get the work done. Users should call fixVariableReference() when AST is complete and all
 variable declarations are in place.
*/
int fixVariableReferences(SgNode* root);

//!Patch up symbol, scope, and parent information when a SgVariableDeclaration's scope is known.
/*!
It is possible to build a variable declaration without knowing its scope information during bottom-up construction of AST, though top-down construction is recommended in general.
In this case, we have to patch up symbol table, scope and parent information when the scope is known. This function is usually used internally within appendStatment(), insertStatement().
*/
void fixVariableDeclaration(SgVariableDeclaration* varDecl, SgScopeStatement* scope);

//! Fix symbols, parent and scope pointers. Used internally within appendStatment(), insertStatement() etc when a struct declaration was built without knowing its target scope.
void fixStructDeclaration(SgClassDeclaration* structDecl, SgScopeStatement* scope);
//! Fix symbols, parent and scope pointers. Used internally within appendStatment(), insertStatement() etc when a class declaration was built without knowing its target scope.
void fixClassDeclaration(SgClassDeclaration* classDecl, SgScopeStatement* scope);

//! Fix symbols, parent and scope pointers. Used internally within appendStatment(), insertStatement() etc when a namespace declaration was built without knowing its target scope.
void fixNamespaceDeclaration(SgNamespaceDeclarationStatement* structDecl, SgScopeStatement* scope);


//! Fix symbol table for SgLabelStatement. Used Internally when the label is built without knowing its target scope. Both parameters cannot be NULL.
void fixLabelStatement(SgLabelStatement* label_stmt, SgScopeStatement* scope);

//! Set a numerical label for a Fortran statement. The statement should have a enclosing function definition already. SgLabelSymbol and SgLabelRefExp are created transparently as needed.
void setFortranNumericLabel(SgStatement* stmt, int label_value);

//! Suggest next usable (non-conflicting) numeric label value for a Fortran function definition scope
int  suggestNextNumericLabel(SgFunctionDefinition* func_def);

//! A wrapper containing fixes (fixVariableDeclaration(),fixStructDeclaration(), fixLabelStatement(), etc) for all kinds statements. Should be used before attaching the statement into AST.
void fixStatement(SgStatement* stmt, SgScopeStatement* scope);
//@}

//! Update defining and nondefining links due to a newly introduced function declaration. Should be used after inserting the function into a scope.
/*! This function not only set the defining and nondefining links of the newly introduced
 *  function declaration inside a scope, but also update other same function declarations' links
 *  accordingly if there are any.
 *  Assumption: The function has already inserted/appended/prepended into the scope before calling this function.
 */
void updateDefiningNondefiningLinks(SgFunctionDeclaration* func, SgScopeStatement* scope);

//------------------------------------------------------------------------
//@{
/*! @name Advanced AST transformations, analyses, and optimizations
  \brief Some complex but commonly used AST transformations.
  */

//! Collect all read and write references within stmt, which can be a function, a scope statement, or a single statement. Note that a reference can be both read and written, like i++
bool
collectReadWriteRefs(SgStatement* stmt, std::vector<SgNode*>& readRefs, std::vector<SgNode*>& writeRefs);

//!Collect unique variables which are read or written within a statement. Note that a variable can be both read and written. The statement can be either of a function, a scope, or a single line statement.
bool collectReadWriteVariables(SgStatement* stmt, std::set<SgInitializedName*>& readVars, std::set<SgInitializedName*>& writeVars);

//!Collect read only variables within a statement. The statement can be either of a function, a scope, or a single line statement.
void collectReadOnlyVariables(SgStatement* stmt, std::set<SgInitializedName*>& readOnlyVars);

//!Collect read only variable symbols within a statement. The statement can be either of a function, a scope, or a single line statement.
void collectReadOnlySymbols(SgStatement* stmt, std::set<SgVariableSymbol*>& readOnlySymbols);

//! Check if a variable reference is used by its address: including &a expression and foo(a) when type2 foo(Type& parameter) in C++
bool isUseByAddressVariableRef(SgVarRefExp* ref);

//! Collect variable references involving use by address: including &a expression and foo(a) when type2 foo(Type& parameter) in C++
void collectUseByAddressVariableRefs (const SgStatement* s, std::set<SgVarRefExp* >& varSetB);

//!Call liveness analysis on an entire project
LivenessAnalysis * call_liveness_analysis(SgProject* project, bool debug=false);

//!get liveIn and liveOut variables for a for loop from liveness analysis result liv.
void getLiveVariables(LivenessAnalysis * liv, SgForStatement* loop, std::set<SgInitializedName*>& liveIns, std::set<SgInitializedName*> & liveOuts);

//!Recognize and collect reduction variables and operations within a C/C++ loop, following OpenMP 3.0 specification for allowed reduction variable types and operation types.
void ReductionRecognition(SgForStatement* loop, std::set< std::pair <SgInitializedName*, VariantT> > & results);

//! Constant folding an AST subtree rooted at 'r' (replacing its children with their constant values, if applicable). Please be advised that constant folding on floating point computation may decrease the accuracy of floating point computations!
/*! It is a wrapper function for ConstantFolding::constantFoldingOptimization(). Note that only r's children are replaced with their corresponding constant values, not the input SgNode r itself. You have to call this upon an expression's parent node if you want to fold the expression. */
void constantFolding(SgNode* r);

//!Instrument(Add a statement, often a function call) into a function right before the return points, handle multiple return statements and return expressions with side effects. Return the number of statements inserted.
/*! Useful when adding a runtime library call to terminate the runtime system right before the end of a program, especially for OpenMP and UPC runtime systems. Return with complex expressions with side effects are rewritten using an additional assignment statement.
 */
int instrumentEndOfFunction(SgFunctionDeclaration * func, SgStatement* s);

//! Remove jumps whose label is immediately after the jump.  Used to clean up inlined code fragments.
void removeJumpsToNextStatement(SgNode*);

//! Remove labels which are not targets of any goto statements
void removeUnusedLabels(SgNode* top);

//! Remove consecutive labels
void removeConsecutiveLabels(SgNode* top);

//! Replace an expression with a temporary variable and an assignment statement
/*!
 Add a new temporary variable to contain the value of 'from'
 Change reference to 'from' to use this new variable
 Assumptions: 'from' is not within the test of a loop or 'if'
              not currently traversing 'from' or the statement it is in

 */
 SgAssignInitializer* splitExpression(SgExpression* from, std::string newName = "");

//! Split long expressions into blocks of statements
void splitExpressionIntoBasicBlock(SgExpression* expr);

//! Remove labeled goto statements
void removeLabeledGotos(SgNode* top);

//! If the given statement contains any break statements in its body, add a new label below the statement and change the breaks into gotos to that new label.
void changeBreakStatementsToGotos(SgStatement* loopOrSwitch);

//! Check if the body of a 'for' statement is a SgBasicBlock, create one if not.
SgBasicBlock* ensureBasicBlockAsBodyOfFor(SgForStatement* fs);

//! Check if the body of a 'for' statement is a SgBasicBlock, create one if not. (10nov17: PP for RTED/upc)
SgBasicBlock* ensureBasicBlockAsBodyOfUpcForAll(SgUpcForAllStatement* fs);

//! Check if the body of a 'while' statement is a SgBasicBlock, create one if not.
SgBasicBlock* ensureBasicBlockAsBodyOfWhile(SgWhileStmt* ws);
//! Check if the body of a 'do .. while' statement is a SgBasicBlock, create one if not.
SgBasicBlock* ensureBasicBlockAsBodyOfDoWhile(SgDoWhileStmt* ws);
//! Check if the body of a 'switch' statement is a SgBasicBlock, create one if not.
SgBasicBlock* ensureBasicBlockAsBodyOfSwitch(SgSwitchStatement* ws);
//! Check if the true body of a 'if' statement is a SgBasicBlock, create one if not.
SgBasicBlock* ensureBasicBlockAsTrueBodyOfIf(SgIfStmt* ifs);
//! Check if the false body of a 'if' statement is a SgBasicBlock, create one if not.
SgBasicBlock* ensureBasicBlockAsFalseBodyOfIf(SgIfStmt* ifs);
SgBasicBlock* ensureBasicBlockAsBodyOfCatch(SgCatchOptionStmt* cos);
//! Check if the body of a SgOmpBodyStatement is a SgBasicBlock, create one if not
SgBasicBlock* ensureBasicBlockAsBodyOfOmpBodyStmt(SgOmpBodyStatement* ompbodyStmt);
/** A wrapper of all ensureBasicBlockAs*() above to ensure the parent of s is a scope statement with list of statements as children,
  * otherwise generate a SgBasicBlock in between. If s is the body of a loop, catch, or if statement and is already
  * a basic block, s is returned unmodified. Else, the (potentially new) parent of s is returned. */
SgLocatedNode* ensureBasicBlockAsParent(SgStatement* s);

//! Fix up ifs, loops, etc. to have blocks as all components and add dummy else
//! clauses to if statements that don't have them
void changeAllLoopBodiesToBlocks(SgNode* top);

//! Get the constant value from a constant integer expression; abort on
//! everything else.  Note that signed long longs are converted to unsigned.
unsigned long long getIntegerConstantValue(SgValueExp* expr);

//! Get a statement's dependent declarations which declares the types used in the statement. The returned vector of declaration statements are sorted according to their appearance order in the original AST. Any reference to a class or template class from a namespace will treated as a reference to the enclosing namespace.
std::vector<SgDeclarationStatement*> getDependentDeclarations (SgStatement* stmt );

//@}


#if 0

//------------------------AST dump, stringify-----------------------------
//------------------------------------------------------------------------
  std::string buildOperatorString ( SgNode* astNode ); //transformationSupport.h

  // do we need these?
  std::string dump_node(const SgNode* astNode);
  std::string dump_tree(const SgNode* astNode);

  // or a friendly version of  unparseToString(), as a memeber function
  std::string SgNode::toString(bool asSubTree=true); // dump node or subtree

//----------------------------AST comparison------------------------------
//------------------------------------------------------------------------
// How to get generic functions for comparison?
  bool isNodeEqual(SgNode* node1, SgNode* node2); //?
  bool isTreeEqual(SgNode* tree1, SgNode* tree2);

	//! Are two expressions equal (using a deep comparison)?
  bool expressionTreeEqual(SgExpression*, SgExpression*);
	//! Are corresponding expressions in two lists equal (using a deep comparison)?
  bool expressionTreeEqualStar(const SgExpressionPtrList&,
                             const SgExpressionPtrList&);

//----------------------AST verfication/repair----------------------------
//------------------------------------------------------------------------
// sanity check of AST subtree, any suggestions?
//  TODO
    verifySgNode(SgNode* node, bool subTree=true);
  //src/midend/astDiagnostics/AstConsistencyTests.h
  // AstTests::runAllTests(SgProject * )

  //src/midend/astUtil/astInterface/AstInterface.h.C
  //FixSgProject(SgProject &project)
  //FixSgTree(SgNode* r)

  //src/frontend/SageIII/astPostProcessing
  //AstPostProcessing(SgNode * node)

//--------------------------AST modification------------------------------
//------------------------------------------------------------------------
// any operations changing AST tree, including
// insert, copy, delete(remove), replace

  // insert before or after some point, argument list is consistent with LowLevelRewrite
  void insertAst(SgNode* targetPosition, SgNode* newNode, bool insertBefore=true);

  // previous examples
  //void myStatementInsert(SgStatement* target,...)
  // void AstInterfaceBase::InsertStmt(AstNodePtr const & orig, AstNodePtr const &n, bool insertbefore, bool extractfromBasicBlock)

  // copy
  // copy children of one basic block to another basic block
  //void appendStatementCopy (const SgBasicBlock* a, SgBasicBlock* b);
  void copyStatements (const SgBasicBlock* src, SgBasicBlock* dst);

  // delete (remove) a node or a whole subtree
  void removeSgNode(SgNode* targetNode); // need this?
  void removeSgNodeTree(SgNode* subtree); // need this?

  void removeStatement( SgStatement* targetStmt);

  //Move = delete + insert
  void moveAst (SgNode* src, SgNode* target); // need this?
      // similar to
  void moveStatements (SgBasicBlock* src, SgBasicBlock* target);

  // replace= delete old + insert new (via building or copying)

// DQ (1/25/2010): This does not appear to exist as a definition anywhere in ROSE.
// void replaceAst(SgNode* oldNode, SgNode* newNode);

   //void replaceChild(SgNode* parent, SgNode* from, SgNode* to);
   //bool AstInterface::ReplaceAst( const AstNodePtr& orig, const AstNodePtr& n)

//--------------------------AST transformations---------------------------
//------------------------------------------------------------------------
// Advanced AST modifications through basic AST modifications
// Might not be included in AST utitlity list, but listed here for the record.

  // extract statements/content from a scope
  void flattenBlocks(SgNode* n);

  //src/midend/astInlining/inlinerSupport.h
  void renameVariables(SgNode* n);
  void renameLabels(SgNode* n, SgFunctionDefinition* enclosingFunctionDefinition);

  void simpleCopyAndConstantPropagation(SgNode* top);
  void changeAllMembersToPublic(SgNode* n);

  void removeVariableDeclaration(SgInitializedName* initname);

  //! Convert something like "int a = foo();" into "int a; a = foo();"
  SgAssignOp* convertInitializerIntoAssignment(SgAssignInitializer* init);

  //! Rewrites a while or for loop so that the official test is changed to
  //! "true" and what had previously been the test is now an if-break
  //! combination (with an inverted condition) at the beginning of the loop
  //! body
  void pushTestIntoBody(LoopStatement* loopStmt);

  //programTransformation/finiteDifferencing/finiteDifferencing.h
  //! Move variables declared in a for statement to just outside that statement.
  void moveForDeclaredVariables(SgNode* root);

//------------------------ Is/Has functions ------------------------------
//------------------------------------------------------------------------
// misc. boolean functions
// some of them could moved to SgXXX class as a member function

  bool isOverloaded (SgFunctionDeclaration * functionDeclaration);

  bool isSwitchCond (const SgStatement* s);
  bool isIfCond (const SgStatement* s);
  bool isWhileCond (const SgStatement* s);
  bool isStdNamespace (const SgScopeStatement* scope);
  bool isTemplateInst (const SgDeclarationStatement* decl);


  bool isCtor (const SgFunctionDeclaration* func);
  bool isDtor (const SgFunctionDeclaration* func);

   // src/midend/astInlining/typeTraits.h
  bool hasTrivialDestructor(SgType* t);
  bool isNonconstReference(SgType* t);
  bool isReferenceType(SgType* t);

  //  generic ones, or move to the SgXXX class as a member function

  bool isConst(SgNode* node); // const type, variable, function, etc.
  // .... and more

  bool isConstType (const SgType* type);
  bool isConstFunction (const SgFunctionDeclaration* decl);


  bool isMemberVariable(const SgInitializedName & var);
  //bool isMemberVariable(const SgNode& in);

  bool isPrototypeInScope (SgScopeStatement * scope,
                           SgFunctionDeclaration * functionDeclaration,
			   SgDeclarationStatement * startingAtDeclaration);

  bool MayRedefined(SgExpression* expr, SgNode* root);
  // bool isPotentiallyModified(SgExpression* expr, SgNode* root); // inlinderSupport.h
  bool hasAddressTaken(SgExpression* expr, SgNode* root);

  //src/midend/astInlining/inlinerSupport.C
     // can also classified as topdown search
  bool containsVariableReference(SgNode* root, SgInitializedName* var);

  bool isDeclarationOf(SgVariableDeclaration* decl, SgInitializedName* var);
  bool isPotentiallyModifiedDuringLifeOf(SgBasicBlock* sc,
                                       SgInitializedName* toCheck,
                                       SgInitializedName* lifetime)
  //src/midend/programTransformation/partialRedundancyElimination/pre.h
  bool anyOfListPotentiallyModifiedIn(const std::vector<SgVariableSymbol*>& syms, SgNode* n);

//------------------------ loop handling ---------------------------------
//------------------------------------------------------------------------
  //get and set loop control expressions
  // 0: init expr, 1: condition expr, 2: stride expr

  SgExpression* getForLoopTripleValues(int valuetype,SgForStatement* forstmt );
  int setForLoopTripleValues(int valuetype,SgForStatement* forstmt, SgExpression* exp);

  bool isLoopIndexVarRef(SgForStatement* forstmt, SgVarRefExp *varref);
  SgInitializedName * getLoopIndexVar(SgForStatement* forstmt);

//------------------------expressions-------------------------------------
//------------------------------------------------------------------------
  //src/midend/programTransformation/partialRedundancyElimination/pre.h
  int countComputationsOfExpressionIn(SgExpression* expr, SgNode* root);

  	//src/midend/astInlining/replaceExpressionWithStatement.h
  void replaceAssignmentStmtWithStatement(SgExprStatement* from, StatementGenerator* to);

  void replaceSubexpressionWithStatement(SgExpression* from,
                                       StatementGenerator* to);
  SgExpression* getRootOfExpression(SgExpression* n);

//--------------------------preprocessing info. -------------------------
//------------------------------------------------------------------------
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
   *  Moves all preprocessing information attached 'before' the source
   *  statement to the front of the destination statement.
   */
  // a generic one for all
  /// void movePreprocessingInfo(src, dest, RelativePositionType);
  void moveBeforePreprocInfo (SgStatement* src, SgStatement* dest);
  void moveInsidePreprocInfo (SgBasicBlock* src, SgBasicBlock* dest);
  void moveAfterPreprocInfo (SgStatement* src, SgStatement* dest);

//--------------------------------operator--------------------------------
//------------------------------------------------------------------------
  from transformationSupport.h, not sure if they should be included here
  /* return enum code for SAGE operators */
  operatorCodeType classifyOverloadedOperator(); // transformationSupport.h

 /*! \brief generates a source code string from operator name.
    This function returns a string representing the elementwise operator (for primative types)
    that would be match that associated with the overloaded operator for a user-defined
    abstractions (e.g. identifyOperator("operator+()") returns "+").
  */
  std::string stringifyOperator (std::string name);

//--------------------------------macro ----------------------------------
//------------------------------------------------------------------------
  std::string buildMacro ( std::string s ); //transformationSupport.h

//--------------------------------access functions---------------------------
//----------------------------------get/set sth.-----------------------------
// several categories:
* get/set a direct child/grandchild node or fields
* get/set a property flag value
* get a descendent child node using preorder searching
* get an ancestor node using bottomup/reverse searching

  	// SgName or string?
  std::string getFunctionName (SgFunctionCallExp* functionCallExp);
  std::string getFunctionTypeName ( SgFunctionCallExp* functionCallExpression );

    // do we need them anymore? or existing member functions are enought?
    // a generic one:
   std::string get_name (const SgNode* node);
   std::string get_name (const SgDeclarationStatement * declaration);

  // get/set some property: should moved to SgXXX as an inherent memeber function?
  // access modifier
  void  setExtern (SgFunctionDeclartion*)
  void  clearExtern()

   // similarly for other declarations and other properties
  void setExtern (SgVariableDeclaration*)
  void setPublic()
  void setPrivate()


#endif
}// end of namespace

#endif
