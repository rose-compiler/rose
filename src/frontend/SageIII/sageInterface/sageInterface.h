#ifndef ROSE_SAGE_INTERFACE
#define ROSE_SAGE_INTERFACE

#include "sage3basic.hhh"
#include <stdint.h>
#include <utility>

#include "rosePublicConfig.h" // for ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
#include "OmpAttribute.h"


#if 0   // FMZ(07/07/2010): the argument "nextErrorCode" should be call-by-reference
SgFile* determineFileType ( std::vector<std::string> argv, int nextErrorCode, SgProject* project );
#else
SgFile* determineFileType ( std::vector<std::string> argv, int& nextErrorCode, SgProject* project );
#endif

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#include "rewrite.h"
#endif

// DQ (7/20/2008): Added support for unparsing abitrary strings in the unparser.
#include "astUnparseAttribute.h"
#include <set>

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#include "LivenessAnalysis.h"
#include "abstract_handle.h"
#include "ClassHierarchyGraph.h"
#endif

// DQ (8/19/2004): Moved from ROSE/src/midend/astRewriteMechanism/rewrite.h
//! A global function for getting the string associated with an enum (which is defined in global scope)
ROSE_DLL_API std::string getVariantName (VariantT v);

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


/** Functions that are useful when operating on the AST.
 *
 *  The Sage III IR design attempts to be minimalist. Thus additional functionality is intended to be presented using separate
 *  higher level interfaces which work with the IR.  This namespace collects functions that operate on the IR and support
 *  numerous types of operations that are common to general analysis and transformation of the AST. */
namespace SageInterface
   {
  // Liao 6/22/2016: keep records of loop init-stmt normalization, later help undo it to support autoPar.
  struct Transformation_Record
  {
    // a lookup table to check if a for loop has been normalized for its c99-style init-stmt
    std::map <SgForStatement* , bool > forLoopInitNormalizationTable; 
    // Detailed record about the original declaration (1st in the pair) and the normalization generated new declaration (2nd in the pair)
    std::map <SgForStatement* , std::pair<SgVariableDeclaration*, SgVariableDeclaration*>  >  forLoopInitNormalizationRecord;
  } ;

  ROSE_DLL_API extern Transformation_Record trans_records;

  // DQ (4/3/2014): Added general AST support separate from the AST.

  // Container and API for analysis information that is outside of the AST and as a result
  // prevents frequent modification of the IR.
     class DeclarationSets
        {
       // DQ (4/3/2014): This stores all associated declarations as a map of sets.
       // the key to the map is the first nondefining declaration and the elements of the set are
       // all of the associated declarations (including the defining declaration).

          private:
           //! Map of first-nondefining declaration to all other associated declarations.
               std::map<SgDeclarationStatement*,std::set<SgDeclarationStatement*>* > declarationMap;

          public:
               void addDeclaration(SgDeclarationStatement* decl);
               const std::set<SgDeclarationStatement*>* getDeclarations(SgDeclarationStatement* decl);

               std::map<SgDeclarationStatement*,std::set<SgDeclarationStatement*>* > & getDeclarationMap();

               bool isLocatedInDefiningScope(SgDeclarationStatement* decl);

        };

  // DQ (4/3/2014): This constructs a data structure that holds analysis information about
  // the AST that is separate from the AST.  This is intended to be a general mechanism 
  // to support analysis information without constantly modifying the IR.
     DeclarationSets* buildDeclarationSets(SgNode*);

//! An internal counter for generating unique SgName
ROSE_DLL_API extern int gensym_counter;

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
//! Find the main interpretation.
SgAsmInterpretation* getMainInterpretation(SgAsmGenericFile* file);

//! Get the unsigned value of a disassembled constant.
uint64_t getAsmConstant(SgAsmValueExpression* e);

//! Get the signed value of a disassembled constant.
int64_t getAsmSignedConstant(SgAsmValueExpression *e);
#endif

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

 //------------------------------------------------------------------------
 //@{
 /*! @name Symbol tables
   \brief  utility functions for symbol tables
 */
   // Liao 1/22/2008, used for get symbols for generating variable reference nodes
   // ! Find a variable symbol in current and ancestor scopes for a given name
   ROSE_DLL_API SgVariableSymbol *lookupVariableSymbolInParentScopes (const SgName & name, SgScopeStatement *currentScope=NULL);

// DQ (8/21/2013): Modified to make newest function parameters be default arguments.
// DQ (8/16/2013): For now we want to remove the use of default parameters and add the support for template parameters and template arguments.
   //! Find a symbol in current and ancestor scopes for a given variable name, starting from top of ScopeStack if currentscope is not given or NULL.
// SgSymbol *lookupSymbolInParentScopes (const SgName & name, SgScopeStatement *currentScope=NULL);
// SgSymbol *lookupSymbolInParentScopes (const SgName & name, SgScopeStatement *currentScope, SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateArgumentList);
   ROSE_DLL_API SgSymbol *lookupSymbolInParentScopes (const SgName & name, SgScopeStatement *currentScope = NULL, SgTemplateParameterPtrList* templateParameterList = NULL, SgTemplateArgumentPtrList* templateArgumentList = NULL);

   // DQ (11/24/2007): Functions moved from the Fortran support so that they could be called from within astPostProcessing.
   //!look up the first matched function symbol in parent scopes given only a function name, starting from top of ScopeStack if currentscope is not given or NULL
   ROSE_DLL_API SgFunctionSymbol *lookupFunctionSymbolInParentScopes (const SgName & functionName, SgScopeStatement *currentScope=NULL);

   // Liao, 1/24/2008, find exact match for a function
   //!look up function symbol in parent scopes given both name and function type, starting from top of ScopeStack if currentscope is not given or NULL
   ROSE_DLL_API SgFunctionSymbol *lookupFunctionSymbolInParentScopes (const SgName &  functionName,
                                                         const SgType* t,
                                                         SgScopeStatement *currentScope=NULL);

   ROSE_DLL_API SgFunctionSymbol *lookupTemplateFunctionSymbolInParentScopes (const SgName & functionName, SgFunctionType * ftype, SgTemplateParameterPtrList * tplparams, SgScopeStatement *currentScope=NULL);
   ROSE_DLL_API SgFunctionSymbol *lookupTemplateMemberFunctionSymbolInParentScopes (const SgName & functionName, SgFunctionType * ftype, SgTemplateParameterPtrList * tplparams, SgScopeStatement *currentScope=NULL);

   ROSE_DLL_API SgTemplateVariableSymbol * lookupTemplateVariableSymbolInParentScopes (const SgName & name, SgTemplateParameterPtrList * tplparams, SgTemplateArgumentPtrList* tplargs, SgScopeStatement *currentScope=NULL);


// DQ (8/21/2013): Modified to make newest function parameters be default arguments.
// DQ (8/16/2013): For now we want to remove the use of default parameters and add the support for template parameters and template arguments.
// DQ (5/7/2011): Added support for SgClassSymbol (used in name qualification support).
// SgClassSymbol*     lookupClassSymbolInParentScopes    (const SgName & name, SgScopeStatement *currentScope = NULL);
   ROSE_DLL_API SgClassSymbol*     lookupClassSymbolInParentScopes    (const SgName & name, SgScopeStatement *currentScope = NULL, SgTemplateArgumentPtrList* templateArgumentList = NULL);
   ROSE_DLL_API SgTypedefSymbol*   lookupTypedefSymbolInParentScopes  (const SgName & name, SgScopeStatement *currentScope = NULL);

   ROSE_DLL_API SgNonrealSymbol*   lookupNonrealSymbolInParentScopes  (const SgName & name, SgScopeStatement *currentScope = NULL, SgTemplateParameterPtrList* templateParameterList = NULL, SgTemplateArgumentPtrList* templateArgumentList = NULL);
#if 0
 // DQ (8/13/2013): This function does not make since any more, now that we have made the symbol
 // table handling more precise and we have to provide template parameters for any template lookup.
 // We also have to know if we want to lookup template classes, template functions, or template 
 // member functions (since each have specific requirements).
   SgTemplateSymbol*  lookupTemplateSymbolInParentScopes (const SgName & name, SgScopeStatement *currentScope = NULL);
#endif
#if 0
// DQ (8/13/2013): I am not sure if we want this functions in place of lookupTemplateSymbolInParentScopes.
// Where these are called we might not know enough information about the template parameters or function 
// types, for example.
   SgTemplateClassSymbol*           lookupTemplateClassSymbolInParentScopes          (const SgName & name, SgScopeStatement *currentScope = NULL, SgTemplateParameterPtrList* templateParameterList = NULL, SgTemplateArgumentPtrList* templateArgumentList = NULL);
   SgTemplateFunctionSymbol*        lookupTemplateFunctionSymbolInParentScopes       (const SgName & name, SgScopeStatement *currentScope = NULL, SgTemplateParameterPtrList* templateParameterList = NULL);
   SgTemplateMemberFunctionSymbol*  lookupTemplateMemberFunctionSymbolInParentScopes (const SgName & name, SgScopeStatement *currentScope = NULL, SgTemplateParameterPtrList* templateParameterList = NULL);
#endif

// DQ (8/21/2013): Modified to make some of the newest function parameters be default arguments.
// DQ (8/13/2013): I am not sure if we want this functions in place of lookupTemplateSymbolInParentScopes.
   ROSE_DLL_API SgTemplateClassSymbol* lookupTemplateClassSymbolInParentScopes (const SgName &  name, SgTemplateParameterPtrList* templateParameterList, SgTemplateArgumentPtrList* templateArgumentList, SgScopeStatement *cscope = NULL);

   ROSE_DLL_API SgEnumSymbol*      lookupEnumSymbolInParentScopes     (const SgName & name, SgScopeStatement *currentScope = NULL);
   ROSE_DLL_API SgNamespaceSymbol* lookupNamespaceSymbolInParentScopes(const SgName & name, SgScopeStatement *currentScope = NULL);

// DQ (7/17/2011): Added function from cxx branch that I need here for the Java support.
// SgClassSymbol* lookupClassSymbolInParentScopes (const SgName &  name, SgScopeStatement *cscope);

   /*! \brief set_name of symbol in symbol table.

       This function extracts the symbol from the relavant symbol table,
       changes the name (at the declaration) and reinserts it into the
       symbol table.

       \internal  I think this is what this function does, I need to double check.
    */
   // DQ (12/9/2004): Moved this function (by Alin Jula) from being a member of SgInitializedName
   // to this location where it can be a part of the interface for the Sage III AST.
   ROSE_DLL_API int set_name (SgInitializedName * initializedNameNode, SgName new_name);

   /*! \brief Output function type symbols in global function type symbol table.
    */
   void outputGlobalFunctionTypeSymbolTable ();

   // DQ (6/27/2005):
   /*! \brief Output the local symbol tables.

       \implementation Each symbol table is output with the file infor where it is located in the source code.
    */
   ROSE_DLL_API void outputLocalSymbolTables (SgNode * node);

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

   /*! \brief Clear those variable symbols with unknown type (together with initialized names) which are also not referenced by any variable references or declarations under root. If root is NULL, all symbols with unknown type will be deleted.
    */
   void clearUnusedVariableSymbols (SgNode* root = NULL);

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

  // DQ (3/20/2016): Added to refactor some of the DSL infrastructure support.
   /*! \brief Generate a useful name to support construction of identifiers from declarations.

       This function permits names to be generated that will be unique across translation units
       (a specific requirement different from the context of the get_name() functions above).

       \internal This supports only a restricted set of declarations presently.
    */
     std::string generateUniqueNameForUseAsIdentifier ( SgDeclarationStatement* declaration );
     std::string generateUniqueNameForUseAsIdentifier_support ( SgDeclarationStatement* declaration );

   /*! \brief Global map of name collisions to support generateUniqueNameForUseAsIdentifier() function.
    */
     extern std::map<std::string,int>     local_name_collision_map;
     extern std::map<std::string,SgNode*> local_name_to_node_map;
     extern std::map<SgNode*,std::string> local_node_to_name_map;

   /*! \brief Traversal to set the global map of names to node and node to names.collisions to support generateUniqueNameForUseAsIdentifier() function.
    */
     void computeUniqueNameForUseAsIdentifier( SgNode* astNode );

   /*! \brief Reset map variables used to support generateUniqueNameForUseAsIdentifier() function.
    */
     void reset_name_collision_map();

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
   ROSE_DLL_API SgMemberFunctionDeclaration *getDefaultConstructor (SgClassDeclaration *
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
   //! Check if a SgNode is a declaration for a union
   bool isUnionDeclaration(SgNode * node);
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

  //! Recursively print current and parent nodes. used within gdb to probe the context of a node.
  void recursivePrintCurrentAndParent (SgNode* n) ;

   //! Save AST into a pdf file. Start from a node to find its enclosing file node. The entire file's AST will be saved into a pdf.
   void saveToPDF(SgNode* node, std::string filename);
   void saveToPDF(SgNode* node); // enable calling from gdb

 // DQ (2/12/2012): Added some diagnostic support.
//! Diagnostic function for tracing back through the parent list to understand at runtime where in the AST a failure happened.
   void whereAmI(SgNode* node);

   //! Extract a SgPragmaDeclaration's leading keyword . For example "#pragma omp parallel" has a keyword of "omp".
   std::string extractPragmaKeyword(const SgPragmaDeclaration *);

   //! Check if a node is SgOmp*Statement
   ROSE_DLL_API bool isOmpStatement(SgNode* );
   /*! \brief Return true if function is overloaded.
    */
   // DQ (8/27/2005):
   bool isOverloaded (SgFunctionDeclaration * functionDeclaration);

// DQ (2/14/2012): Added support function used for variable declarations in conditionals.
//! Support function used for variable declarations in conditionals
   void initializeIfStmt(SgIfStmt *ifstmt, SgStatement* conditional, SgStatement * true_body, SgStatement * false_body);

//! Support function used for variable declarations in conditionals
   void initializeSwitchStatement(SgSwitchStatement* switchStatement,SgStatement *item_selector,SgStatement *body);

//! Support function used for variable declarations in conditionals
   void initializeWhileStatement(SgWhileStmt* whileStatement, SgStatement *  condition, SgStatement *body, SgStatement *else_body);

  //! Generate unique names for expressions and attach the names as persistent attributes ("UniqueNameAttribute")
   void annotateExpressionsWithUniqueNames (SgProject* project);

   //! Check if a SgNode is a main() function declaration
   ROSE_DLL_API bool isMain (const SgNode* node);
   // DQ (6/22/2005):
   /*! \brief Generate unique name from C and C++ constructs. The name may contain space.

       This is support for the AST merge, but is generally useful as a more general mechanism than
       name mangling which is more closely ties to the generation of names to support link-time function name
       resolution.  This is more general than common name mangling in that it resolves more relevant differences
       between C and C++ declarations. (e.g. the type within the declaration: "struct { int:8; } foo;").

      \implementation current work does not support expressions.

   */
    std::string generateUniqueName ( const SgNode * node, bool ignoreDifferenceBetweenDefiningAndNondefiningDeclarations);

    /** Generate a name like __temp#__ that is unique in the current scope and any parent and children scopes. # is a unique integer counter.
    * @param baseName the word to be included in the variable names. */
    std::string generateUniqueVariableName(SgScopeStatement* scope, std::string baseName = "temp");

  // DQ (8/10/2010): Added const to first parameter.
  // DQ (3/10/2007):
  //! Generate a unique string from the source file position information
    std::string declarationPositionString (const SgDeclarationStatement * declaration);

  // DQ (1/20/2007):
  //! Added mechanism to generate project name from list of file names
    ROSE_DLL_API std::string generateProjectName (const SgProject * project, bool supressSuffix = false );

  //! Given a SgExpression that represents a named function (or bound member
  //! function), return the mentioned function
  SgFunctionDeclaration* getDeclarationOfNamedFunction(SgExpression* func);

  //! Get the mask expression from the header of a SgForAllStatement
  SgExpression* forallMaskExpression(SgForAllStatement* stmt);

  //! Find all SgPntrArrRefExp under astNode, then add SgVarRefExp (if any) of SgPntrArrRefExp's dim_info into NodeList_t
  void addVarRefExpFromArrayDimInfo(SgNode * astNode, Rose_STL_Container<SgNode *>& NodeList_t);

  // DQ (10/6/2006): Added support for faster mangled name generation (caching avoids recomputation).
  /*! \brief Support for faster mangled name generation (caching avoids recomputation).

   */
#ifndef SWIG
// DQ (3/10/2013): This appears to be a problem for the SWIG interface (undefined reference at link-time).
  void clearMangledNameCache (SgGlobal * globalScope);
  void resetMangledNameCache (SgGlobal * globalScope);
#endif

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
    ROSE_DLL_API std::vector < SgNode * >astIntersection (SgNode * original, SgNode * copy, SgCopyHelp * help = NULL);

  //! Deep copy an arbitrary subtree
   ROSE_DLL_API SgNode* deepCopyNode (const SgNode* subtree);

//! A template function for deep copying a subtree. It is also  used to create deepcopy functions with specialized parameter and return types. e.g SgExpression* copyExpression(SgExpression* e);
   template <typename NodeType>
   NodeType* deepCopy (const NodeType* subtree) {
     return dynamic_cast<NodeType*>(deepCopyNode(subtree));
   }

//! Deep copy an expression
   ROSE_DLL_API SgExpression* copyExpression(SgExpression* e);

//!Deep copy a statement
   ROSE_DLL_API SgStatement* copyStatement(SgStatement* s);

// from VarSym.cc in src/midend/astOutlining/src/ASTtools
//! Get the variable symbol for the first initialized name of a declaration stmt.
  ROSE_DLL_API SgVariableSymbol* getFirstVarSym (SgVariableDeclaration* decl);

//! Get the first initialized name of a declaration statement
  ROSE_DLL_API SgInitializedName* getFirstInitializedName (SgVariableDeclaration* decl);

//! A special purpose statement removal function, originally from inlinerSupport.h, Need Jeremiah's attention to refine it. Please don't use it for now.
ROSE_DLL_API void myRemoveStatement(SgStatement* stmt);

ROSE_DLL_API bool isConstantTrue(SgExpression* e);
ROSE_DLL_API bool isConstantFalse(SgExpression* e);

ROSE_DLL_API bool isCallToParticularFunction(SgFunctionDeclaration* decl, SgExpression* e);
ROSE_DLL_API bool isCallToParticularFunction(const std::string& qualifiedName, size_t arity, SgExpression* e);

//! Check if a declaration has a "static' modifier
bool ROSE_DLL_API isStatic(SgDeclarationStatement* stmt);

//! Set a declaration as static
ROSE_DLL_API void setStatic(SgDeclarationStatement* stmt);

//! Check if a declaration has an "extern" modifier
ROSE_DLL_API bool isExtern(SgDeclarationStatement* stmt);

//! Set a declaration as extern
ROSE_DLL_API void setExtern(SgDeclarationStatement* stmt);

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

//! Variable references can be introduced by SgVarRef, SgPntrArrRefExp, SgInitializedName, SgMemberFunctionRef etc. For Dot and Arrow Expressions, their lhs is used to obtain SgInitializedName (coarse grain) by default. Otherwise, fine-grain rhs is used. 
ROSE_DLL_API SgInitializedName* convertRefToInitializedName(SgNode* current, bool coarseGrain=true);

//! Build an abstract handle from an AST node, reuse previously built handle when possible
ROSE_DLL_API AbstractHandle::abstract_handle* buildAbstractHandle(SgNode*);

//! Obtain a matching SgNode from an abstract handle string
ROSE_DLL_API SgNode* getSgNodeFromAbstractHandleString(const std::string& input_string);

//! Dump information about a SgNode for debugging
ROSE_DLL_API void dumpInfo(SgNode* node, std::string desc="");

//! Reorder a list of declaration statements based on their appearance order in source files
ROSE_DLL_API std::vector<SgDeclarationStatement*>
sortSgNodeListBasedOnAppearanceOrderInSource(const std::vector<SgDeclarationStatement*>& nodevec);

// DQ (4/13/2013): We need these to support the unparing of operators defined by operator syntax or member function names.
//! Is an overloaded operator a prefix operator (e.g. address operator X * operator&(), dereference operator X & operator*(), unary plus operator X & operator+(), etc.
// bool isPrefixOperator( const SgMemberFunctionRefExp* memberFunctionRefExp );
bool isPrefixOperator( SgExpression* exp );

//! Check for proper names of possible prefix operators (used in isPrefixOperator()). 
bool isPrefixOperatorName( const SgName & functionName );

//! Is an overloaded operator a postfix operator. (e.g. ).
bool isPostfixOperator( SgExpression* exp );

//! Is an overloaded operator an index operator (also referred to as call or subscript operators). (e.g. X & operator()() or X & operator[]()).
bool isIndexOperator( SgExpression* exp );

// DQ (1/10/2014): Adding more general support for token based unparsing.
//! Used to support token unparsing (when the output the trailing token sequence).
SgStatement* lastStatementOfScopeWithTokenInfo (SgScopeStatement* scope, std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap);

//@}

//------------------------------------------------------------------------
//@{
/*! @name AST properties
  \brief version, language properties of current AST.
*/

//  std::string version();  // utility_functions.h, version number
  /*! Brief These traverse the memory pool of SgFile IR nodes and determine what languages are in use!
   */
  ROSE_DLL_API bool is_Ada_language ();
  ROSE_DLL_API bool is_C_language ();
  ROSE_DLL_API bool is_Cobol_language ();
  ROSE_DLL_API bool is_OpenMP_language ();
  ROSE_DLL_API bool is_UPC_language ();
  //! Check if dynamic threads compilation is used for UPC programs
  ROSE_DLL_API bool is_UPC_dynamic_threads();
  ROSE_DLL_API bool is_C99_language ();
  ROSE_DLL_API bool is_Cxx_language ();
  ROSE_DLL_API bool is_Java_language ();
  ROSE_DLL_API bool is_Jovial_language ();
  ROSE_DLL_API bool is_Fortran_language ();
  ROSE_DLL_API bool is_CAF_language ();
  ROSE_DLL_API bool is_PHP_language();
  ROSE_DLL_API bool is_Python_language();
  ROSE_DLL_API bool is_Cuda_language();
  ROSE_DLL_API bool is_OpenCL_language();
  ROSE_DLL_API bool is_X10_language();
  ROSE_DLL_API bool is_binary_executable();
  ROSE_DLL_API bool is_mixed_C_and_Cxx_language ();
  ROSE_DLL_API bool is_mixed_Fortran_and_C_language ();
  ROSE_DLL_API bool is_mixed_Fortran_and_Cxx_language ();
  ROSE_DLL_API bool is_mixed_Fortran_and_C_and_Cxx_language ();

  ROSE_DLL_API bool is_language_case_insensitive ();
  ROSE_DLL_API bool language_may_contain_nondeclarations_in_scope ();

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

      This is used to clear the cache of computed unique labels for scopes in a function.
      This function should be called after any transformation on a function that might effect
      the allocation of scopes and cause the existing unique numbers to be incorrect.
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
  bool ROSE_DLL_API isAncestor(SgNode* node1, SgNode* node2);
//@}
//------------------------------------------------------------------------
//@{
/*! @name Preprocessing Information
  \brief #if-#else-#end, comments, #include, etc
*/

  //! Dumps a located node's preprocessing information.
  void dumpPreprocInfo (SgLocatedNode* locatedNode);

//! Insert  #include "filename" or #include <filename> (system header) onto the global scope of a source file, add to be the last #include .. by default among existing headers, Or as the first header. Recommended for use.
PreprocessingInfo * insertHeader(SgSourceFile * source_file, const std::string & header_file_name, bool isSystemHeader, bool asLastHeader);

//! Insert a new header right before stmt,  if there are existing headers attached to stmt, insert it as the last or first header as specified by asLastHeader
void insertHeader (SgStatement* stmt, PreprocessingInfo* newheader, bool asLastHeader);

//! Insert  #include "filename" or #include <filename> (system header) onto the global scope of a source file
PreprocessingInfo * insertHeader(SgSourceFile * source_file, const std::string & header_file_name, bool isSystemHeader = false, PreprocessingInfo::RelativePositionType position = PreprocessingInfo::before);

//! Insert  #include "filename" or #include <filename> (system header) into the global scope containing the current scope, right after other #include XXX.
ROSE_DLL_API PreprocessingInfo* insertHeader(const std::string& filename, PreprocessingInfo::RelativePositionType position=PreprocessingInfo::after, bool isSystemHeader=false, SgScopeStatement* scope=NULL);

//! Identical to movePreprocessingInfo(), except for the stale name and confusing order of parameters. It will be deprecated soon.
ROSE_DLL_API void moveUpPreprocessingInfo (SgStatement* stmt_dst, SgStatement* stmt_src, PreprocessingInfo::RelativePositionType src_position=PreprocessingInfo::undef,  PreprocessingInfo::RelativePositionType dst_position=PreprocessingInfo::undef, bool usePrepend= false);

//! Move preprocessing information of stmt_src to stmt_dst, Only move preprocessing information from the specified source-relative position to a specified target position, otherwise move all preprocessing information with position information intact. The preprocessing information is appended to the existing preprocessing information list of the target node by default. Prepending is used if usePreprend is set to true. Optionally, the relative position can be adjust after the moving using dst_position.
ROSE_DLL_API void movePreprocessingInfo (SgStatement* stmt_src, SgStatement* stmt_dst,  PreprocessingInfo::RelativePositionType src_position=PreprocessingInfo::undef,
                             PreprocessingInfo::RelativePositionType dst_position=PreprocessingInfo::undef, bool usePrepend= false);


//!Cut preprocessing information from a source node and save it into a buffer. Used in combination of pastePreprocessingInfo(). The cut-paste operation is similar to moveUpPreprocessingInfo() but it is more flexible in that the destination node can be unknown during the cut operation.
ROSE_DLL_API void cutPreprocessingInfo (SgLocatedNode* src_node, PreprocessingInfo::RelativePositionType pos, AttachedPreprocessingInfoType& save_buf);

//!Paste preprocessing information from a buffer to a destination node. Used in combination of cutPreprocessingInfo()
ROSE_DLL_API void pastePreprocessingInfo (SgLocatedNode* dst_node, PreprocessingInfo::RelativePositionType pos, AttachedPreprocessingInfoType& saved_buf);

//! Attach an arbitrary string to a located node. A workaround to insert irregular statements or vendor-specific attributes.
ROSE_DLL_API PreprocessingInfo* attachArbitraryText(SgLocatedNode* target,
                const std::string & text,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before);

//!Check if a pragma declaration node has macro calls attached, if yes, replace macro calls within the pragma string with expanded strings. This only works if -rose:wave is turned on.
ROSE_DLL_API void replaceMacroCallsWithExpandedStrings(SgPragmaDeclaration* target);
//@}

//! Build and attach comment onto the global scope of a source file
PreprocessingInfo* attachComment(
  SgSourceFile * source_file,
  const std::string & content,
  PreprocessingInfo::DirectiveType directive_type = PreprocessingInfo::C_StyleComment,
  PreprocessingInfo::RelativePositionType  position = PreprocessingInfo::before
);

//! Build and attach comment, comment style is inferred from the language type of the target node if not provided
   ROSE_DLL_API PreprocessingInfo* attachComment(SgLocatedNode* target, const std::string & content,
               PreprocessingInfo::RelativePositionType position=PreprocessingInfo::before,
               PreprocessingInfo::DirectiveType dtype= PreprocessingInfo::CpreprocessorUnknownDeclaration);

// DQ (7/20/2008): I am not clear were I should put this function, candidates include: SgLocatedNode or SgInterface
//! Add a string to be unparsed to support code generation for back-end specific tools or compilers.
  ROSE_DLL_API void addTextForUnparser ( SgNode* astNode, std::string s, AstUnparseAttribute::RelativePositionType inputlocation );

/**
 * Add preproccessor guard around a given node.
 * It surrounds the node with "#if guard" and "#endif"
 */
void guardNode(SgLocatedNode * target, std::string guard);

//@}


//------------------------------------------------------------------------
//@{
/*! @name Source File Position
  \brief set Sg_File_Info for a SgNode
*/

// ************************************************************************
//              Newer versions of now depricated functions
// ************************************************************************

// DQ (5/1/2012): This function queries the SageBuilder::SourcePositionClassification mode (stored in the SageBuilder 
// interface) and used the specified mode to initialize the source position data (Sg_File_Info objects).  This 
// function is the only function that should be called directly (though in a namespace we can't define permissions).
//! Set the source code positon for the current (input) node.
  ROSE_DLL_API void setSourcePosition(SgNode* node);

// A better name might be "setSourcePositionForSubTree"
//! Set the source code positon for the subtree (including the root).
  ROSE_DLL_API void setSourcePositionAtRootAndAllChildren(SgNode *root);

//! DQ (5/1/2012): New function with improved name.
void setSourcePositionAsTransformation(SgNode *node);

// DQ (5/1/2012): Newly renamed function (previous name preserved for backward compatability).
void setSourcePositionPointersToNull(SgNode *node);

// ************************************************************************



// ************************************************************************
//                  Older deprecated functions
// ************************************************************************
  // Liao, 1/8/2007, set file info. for a whole subtree as transformation generated
//! Set current node's source position as transformation generated
  ROSE_DLL_API void setOneSourcePositionForTransformation(SgNode *node);

//! Set current node's source position as NULL
  ROSE_DLL_API void setOneSourcePositionNull(SgNode *node);

//! Recursively set source position info(Sg_File_Info) as transformation generated
  ROSE_DLL_API void setSourcePositionForTransformation (SgNode * root);

//! Set source position info(Sg_File_Info) as transformation generated for all SgNodes in memory pool
//  ROSE_DLL_API void setSourcePositionForTransformation_memoryPool();

//! Check if a node is from a system header file
  ROSE_DLL_API bool insideSystemHeader (SgLocatedNode* node);

//! Set the source position of SgLocatedNode to Sg_File_Info::generateDefaultFileInfo(). These nodes WILL be unparsed. Not for transformation usage.
// ROSE_DLL_API void setSourcePosition (SgLocatedNode * locatedNode);
// ************************************************************************

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
ROSE_DLL_API bool isStrictIntegerType(SgType* t);
//!Get the data type of the first initialized name of a declaration statement
ROSE_DLL_API SgType* getFirstVarType(SgVariableDeclaration* decl);

//! Is a type default constructible?  This may not quite work properly.
ROSE_DLL_API bool isDefaultConstructible(SgType* type);

//! Is a type copy constructible?  This may not quite work properly.
ROSE_DLL_API bool isCopyConstructible(SgType* type);

//! Is a type assignable?  This may not quite work properly.
ROSE_DLL_API bool isAssignable(SgType* type);

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
//! Check if a class type is a pure virtual class. True means that there is at least
//! one pure virtual function that has not been overridden.
//! In the case of an incomplete class type (forward declaration), this function returns false.
ROSE_DLL_API bool isPureVirtualClass(SgType* type, const ClassHierarchyWrapper& classHierarchy);
#endif

//! Does a type have a trivial (built-in) destructor?
ROSE_DLL_API bool hasTrivialDestructor(SgType* t);

//! Is this type a non-constant reference type? (Handles typedefs correctly)
ROSE_DLL_API bool isNonconstReference(SgType* t);

//! Is this type a const or non-const reference type? (Handles typedefs correctly)
ROSE_DLL_API bool isReferenceType(SgType* t);

//! Is this type a pointer type? (Handles typedefs correctly)
ROSE_DLL_API bool isPointerType(SgType* t);

//! Is this a pointer to a non-const type? Note that this function will return true for const pointers pointing to
//! non-const types. For example, (int* const y) points to a modifiable int, so this function returns true. Meanwhile,
//! it returns false for (int const * x) and (int const * const x) because these types point to a const int.
//! Also, only the outer layer of nested pointers is unwrapped. So the function returns true for (const int ** y), but returns
//! false for const (int * const * x)
ROSE_DLL_API bool isPointerToNonConstType(SgType* type);

//! Is this a const type?
/* const char* p = "aa"; is not treated as having a const type. It is a pointer to const char.
 * Similarly, neither for const int b[10]; or const int & c =10;
 * The standard says, "A compound type is not cv-qualified by the cv-qualifiers (if any) of
the types from which it is compounded. Any cv-qualifiers applied to an array type affect the array element type, not the array type".
 */
ROSE_DLL_API bool isConstType(SgType* t);

//! Remove const (if present) from a type.  stripType() cannot do this because it removes all modifiers.
SgType* removeConst(SgType* t);

//! Is this a volatile type?
ROSE_DLL_API bool isVolatileType(SgType* t);

//! Is this a restrict type?
ROSE_DLL_API bool isRestrictType(SgType* t);

//! Is this a scalar type?
/*! We define the following SgType as scalar types: char, short, int, long , void, Wchar, Float, double, long long, string, bool, complex, imaginary
 */
ROSE_DLL_API bool isScalarType(SgType* t);

//! Check if a type is an integral type, only allowing signed/unsigned short, int, long, long long.
//!
//! There is another similar function named SgType::isIntegerType(), which allows additional types char, wchar, and bool.
ROSE_DLL_API bool isStrictIntegerType(SgType* t);

//! Check if a type is a struct type (a special SgClassType in ROSE)
ROSE_DLL_API bool isStructType(SgType* t);

//! Generate a mangled string for a given type based on Itanium C++ ABI
ROSE_DLL_API std::string mangleType(SgType* type);

//! Generate mangled scalar type names according to Itanium C++ ABI, the input type should pass isScalarType() in ROSE
ROSE_DLL_API std::string mangleScalarType(SgType* type);

//! Generated mangled modifier types, include const, volatile,according to Itanium C++ ABI, with extension to handle UPC shared types.
ROSE_DLL_API std::string mangleModifierType(SgModifierType* type);

//! Calculate the number of elements of an array type: dim1* dim2*... , assume element count is 1 for int a[]; Strip off THREADS if it is a UPC array.
ROSE_DLL_API size_t getArrayElementCount(SgArrayType* t);

//! Get the number of dimensions of an array type
ROSE_DLL_API int getDimensionCount(SgType* t);

//! Get the element type of an array. It recursively find the base type for multi-dimension array types
ROSE_DLL_API SgType* getArrayElementType(SgType* t);

//! Get the element type of an array, pointer or string, or NULL if not applicable. This function only check one level base type. No recursion.
ROSE_DLL_API SgType* getElementType(SgType* t);


/// \brief  returns the array dimensions in an array as defined for arrtype
/// \param  arrtype the type of a C/C++ array
/// \return an array that contains an expression indicating each dimension's size.
///         OWNERSHIP of the expressions is TRANSFERED TO the CALLER (which
///         becomes responsible for freeing the expressions).
///         Note, the first entry of the array is a SgNullExpression, iff the
///         first array dimension was not specified.
/// \code
///         int x[] = { 1, 2, 3 };
/// \endcode
///         note, the expression does not have to be a constant
/// \code
///         int x[i*5];
/// \endcode
/// \post   return-value.empty() == false
/// \post   return-value[*] != NULL (no nullptr in the returned vector)
std::vector<SgExpression*>
get_C_array_dimensions(const SgArrayType& arrtype);

/// \brief  returns the array dimensions in an array as defined for arrtype
/// \param  arrtype the type of a C/C++ array
/// \param  varref  a reference to an array variable (the variable of type arrtype)
/// \return an array that contains an expression indicating each dimension's size.
///         OWNERSHIP of the expressions is TRANSFERED TO the CALLER (which
///         becomes responsible for freeing the expressions).
///         If the first array dimension was not specified an expression
///         that indicates that size is generated.
/// \code
///         int x[][3] = { 1, 2, 3, 4, 5, 6 };
/// \endcode
///         the entry for the first dimension will be:
/// \code
///         // 3 ... size of 2nd dimension
///         sizeof(x) / (sizeof(int) * 3)
/// \endcode
/// \pre    arrtype is the array-type of varref
/// \post   return-value.empty() == false
/// \post   return-value[*] != NULL (no nullptr in the returned vector)
/// \post   !isSgNullExpression(return-value[*])
std::vector<SgExpression*>
get_C_array_dimensions(const SgArrayType& arrtype, const SgVarRefExp& varref);

/// \overload
/// \note     see get_C_array_dimensions for SgVarRefExp for details.
/// \todo     make initname const
std::vector<SgExpression*>
get_C_array_dimensions(const SgArrayType& arrtype, SgInitializedName& initname);


//! Check if an expression is an array access (SgPntrArrRefExp). If so, return its name expression and subscripts if requested. Users can use convertRefToInitializedName() to get the possible name. It does not check if the expression is a top level SgPntrArrRefExp.
ROSE_DLL_API bool isArrayReference(SgExpression* ref, SgExpression** arrayNameExp=NULL, std::vector<SgExpression*>** subscripts=NULL);


//! Collect variable references in array types. The default NodeQuery::querySubTree() will miss variables referenced in array type's index list. e.g. double *buffer = new double[numItems] ;
ROSE_DLL_API int collectVariableReferencesInArrayTypes (SgLocatedNode* root, Rose_STL_Container<SgNode*> & currentVarRefList);
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
ROSE_DLL_API bool hasUpcSharedType(SgType* t, SgModifierType ** mod_type_out = NULL  );

//! Check if a type is a UPC shared type, including shared array, shared pointers etc. Exclude private pointers to shared types. Optionally return the modifier type with the UPC shared property.
/*!
 * ROSE uses SgArrayType of SgModifierType to represent shared arrays, not SgModifierType points to SgArrayType. Also typedef may cause a chain of nodes before reach the actual SgModifierType with UPC shared property.
 */
ROSE_DLL_API bool isUpcSharedType(SgType* t, SgModifierType ** mod_type_out = NULL);

//! Check if a modifier type is a UPC shared type.
ROSE_DLL_API bool isUpcSharedModifierType (SgModifierType* mod_type);

//! Check if an array type is a UPC shared type. ROSE AST represents a UPC shared array as regular array of elements of UPC shared Modifier Type. Not directly a UPC shared Modifier Type of an array.
ROSE_DLL_API bool isUpcSharedArrayType (SgArrayType* array_type);

//! Check if a shared UPC type is strict memory consistency or not. Return false if it is relaxed. (So isUpcRelaxedSharedModifierType() is not necessary.)
ROSE_DLL_API bool isUpcStrictSharedModifierType(SgModifierType* mode_type);

//! Get the block size of a UPC shared modifier type
ROSE_DLL_API size_t getUpcSharedBlockSize(SgModifierType* mod_type);

//! Get the block size of a UPC shared type, including Modifier types and array of modifier types (shared arrays)
ROSE_DLL_API size_t getUpcSharedBlockSize(SgType* t);

//! Is UPC phase-less shared type? Phase-less means block size of the first SgModifierType with UPC information is 1 or 0/unspecified. Also return false if the type is not a UPC shared type.
ROSE_DLL_API bool isUpcPhaseLessSharedType (SgType* t);

//! Is a UPC private-to-shared pointer?  SgPointerType comes first compared to SgModifierType with UPC information. Input type must be any of UPC shared types first.
ROSE_DLL_API bool isUpcPrivateToSharedType(SgType* t);

//! Is a UPC array with dimension of X*THREADS
ROSE_DLL_API bool isUpcArrayWithThreads(SgArrayType* t);

//! Lookup a named type based on its name, bottomup searching from a specified scope. Note name collison might be allowed for c (not C++) between typedef and enum/struct. Only the first matched named type will be returned in this case. typedef is returned as it is, not the base type it actually refers to.
ROSE_DLL_API SgType* lookupNamedTypeInParentScopes(const std::string& type_name, SgScopeStatement* scope=NULL);

// DQ (7/22/2014): Added support for comparing expression types in actual arguments with those expected from the formal function parameter types.
//! Get the type of the associated argument expression from the function type.
ROSE_DLL_API SgType* getAssociatedTypeFromFunctionTypeList(SgExpression* actual_argument_expression);

//! Verify that 2 SgTemplateArgument are equivalent (same type, same expression, or same template declaration)
ROSE_DLL_API bool templateArgumentEquivalence(SgTemplateArgument * arg1, SgTemplateArgument * arg2);

//! Verify that 2 SgTemplateArgumentPtrList are equivalent.
ROSE_DLL_API bool templateArgumentListEquivalence(const SgTemplateArgumentPtrList & list1, const SgTemplateArgumentPtrList & list2);

//! Test for equivalence of types independent of access permissions (private or protected modes for members of classes).
ROSE_DLL_API bool isEquivalentType (const SgType* lhs, const SgType* rhs);


//! Test if two types are equivalent SgFunctionType nodes. This is necessary for template function types
//! They may differ in one SgTemplateType pointer but identical otherwise. 
ROSE_DLL_API bool isEquivalentFunctionType (const SgFunctionType* lhs, const SgFunctionType* rhs);

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
ROSE_DLL_API void addStepToLoopBody(SgScopeStatement* loopStmt, SgStatement* step);

ROSE_DLL_API void moveForStatementIncrementIntoBody(SgForStatement* f);
ROSE_DLL_API void convertForToWhile(SgForStatement* f);
ROSE_DLL_API void convertAllForsToWhiles(SgNode* top);
//! Change continue statements in a given block of code to gotos to a label
ROSE_DLL_API void changeContinuesToGotos(SgStatement* stmt, SgLabelStatement* label);

//!Return the loop index variable for a for loop
ROSE_DLL_API SgInitializedName* getLoopIndexVariable(SgNode* loop);

//!Check if a SgInitializedName is used as a loop index within a AST subtree
//! This function will use a bottom-up traverse starting from the subtree_root to find all enclosing loops and check if ivar is used as an index for either of them.
ROSE_DLL_API bool isLoopIndexVariable(SgInitializedName* ivar, SgNode* subtree_root);

//! Check if a for loop uses C99 style initialization statement with multiple expressions like for (int i=0, j=0; ..) or for (i=0,j=0;...)
/*!
   for (int i=0, j=0; ..) is stored as two variable declarations under SgForInitStatement's init_stmt member
   for (i=0,j=0;...) is stored as a single expression statement, with comma expression (i=0,j=0).
*/
ROSE_DLL_API bool hasMultipleInitStatmentsOrExpressions (SgForStatement* for_loop);

//! Routines to get and set the body of a loop
ROSE_DLL_API SgStatement* getLoopBody(SgScopeStatement* loop);

ROSE_DLL_API void setLoopBody(SgScopeStatement* loop, SgStatement* body);

//! Routines to get the condition of a loop. It recognize While-loop, For-loop, and Do-While-loop
ROSE_DLL_API SgStatement* getLoopCondition(SgScopeStatement* loop);

//! Set the condition statement of a loop, including While-loop, For-loop, and Do-While-loop.
ROSE_DLL_API void setLoopCondition(SgScopeStatement* loop, SgStatement* cond);

//! Check if a for-loop has a canonical form, return loop index, bounds, step, and body if requested
//!
//! A canonical form is defined as : one initialization statement, a test expression, and an increment expression , loop index variable should be of an integer type.  IsInclusiveUpperBound is true when <= or >= is used for loop condition
ROSE_DLL_API bool isCanonicalForLoop(SgNode* loop, SgInitializedName** ivar=NULL, SgExpression** lb=NULL, SgExpression** ub=NULL, SgExpression** step=NULL, SgStatement** body=NULL, bool *hasIncrementalIterationSpace = NULL, bool* isInclusiveUpperBound = NULL);

//! Check if a Fortran Do loop has a complete canonical form: Do I=1, 10, 1
ROSE_DLL_API bool isCanonicalDoLoop(SgFortranDo* loop,SgInitializedName** ivar/*=NULL*/, SgExpression** lb/*=NULL*/, SgExpression** ub/*=NULL*/, SgExpression** step/*=NULL*/, SgStatement** body/*=NULL*/, bool *hasIncrementalIterationSpace/*= NULL*/, bool* isInclusiveUpperBound/*=NULL*/);

//! Set the lower bound of a loop header for (i=lb; ...)
ROSE_DLL_API void setLoopLowerBound(SgNode* loop, SgExpression* lb);

//! Set the upper bound of a loop header,regardless the condition expression type.  for (i=lb; i op up, ...)
ROSE_DLL_API void setLoopUpperBound(SgNode* loop, SgExpression* ub);

//! Set the stride(step) of a loop 's incremental expression, regardless the expression types (i+=s; i= i+s, etc)
ROSE_DLL_API void setLoopStride(SgNode* loop, SgExpression* stride);


//! Normalize loop init stmt by promoting the single variable declaration statement outside of the for loop header's init statement, e.g. for (int i=0;) becomes int i_x; for (i_x=0;..) and rewrite the loop with the new index variable, if necessary
ROSE_DLL_API bool normalizeForLoopInitDeclaration(SgForStatement* loop);

//! Undo the normalization of for loop's C99 init declaration. Previous record of normalization is used to ease the reverse transformation.
ROSE_DLL_API bool unnormalizeForLoopInitDeclaration(SgForStatement* loop);

//! Normalize a for loop, return true if successful. Generated constants will be fold by default.
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
ROSE_DLL_API bool forLoopNormalization(SgForStatement* loop, bool foldConstant = true);

//! Normalize a for loop's test expression 
//!           i<x is normalized to i<= (x-1) and
//!           i>x is normalized to i>= (x+1)
ROSE_DLL_API bool normalizeForLoopTest(SgForStatement* loop);
ROSE_DLL_API bool normalizeForLoopIncrement(SgForStatement* loop);

//!Normalize a Fortran Do loop. Make the default increment expression (1) explicit
ROSE_DLL_API bool doLoopNormalization(SgFortranDo* loop);

//!  Unroll a target loop with a specified unrolling factor. It handles steps larger than 1 and adds a fringe loop if the iteration count is not evenly divisible by the unrolling factor.
ROSE_DLL_API bool loopUnrolling(SgForStatement* loop, size_t unrolling_factor);

//! Interchange/permutate a n-level perfectly-nested loop rooted at 'loop' using a lexicographical order number within (0,depth!).
ROSE_DLL_API bool loopInterchange(SgForStatement* loop, size_t depth, size_t lexicoOrder);

//! Tile the n-level (starting from 1) loop of a perfectly nested loop nest using tiling size s
ROSE_DLL_API bool loopTiling(SgForStatement* loopNest, size_t targetLevel, size_t tileSize);

//Winnie Loop Collapsing
SgExprListExp * loopCollapsing(SgForStatement* target_loop, size_t collapsing_factor);

bool getForLoopInformations(
  SgForStatement * for_loop,
  SgVariableSymbol * & iterator,
  SgExpression * & lower_bound,
  SgExpression * & upper_bound,
  SgExpression * & stride
);

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
#if 0
     printf ("Top of SageInterface::querySubTree() \n");
#endif

     Rose_STL_Container<SgNode*> nodes = NodeQuery::querySubTree(top,variant);
     std::vector<NodeType*> result(nodes.size(), NULL);
     int count = 0;
#if 0
     printf ("In SageInterface::querySubTree(): before initialization loop \n");
#endif

     for (Rose_STL_Container<SgNode*>::const_iterator i = nodes.begin(); i != nodes.end(); ++i, ++count) 
        {
#if 0
          printf ("In SageInterface::querySubTree(): in loop: count = %d \n",count);
#endif
          NodeType* node = dynamic_cast<NodeType*>(*i);
          ROSE_ASSERT (node);
          result[count] = node;
        }
#if 0
     printf ("Leaving SageInterface::querySubTree(): after initialization loop \n");
#endif

     return result;
   }
  /*! \brief Returns STL vector of SgFile IR node pointers.

      Demonstrates use of restricted traversal over just SgFile IR nodes.
   */
    std::vector < SgFile * >generateFileList ();

/** Get the current SgProject IR Node.
 *
 *  The library should never have more than one project and it asserts such.  If no project has been created yet then this
 *  function returns the null pointer. */
ROSE_DLL_API SgProject * getProject();

  //! \return the project associated with a node
  SgProject * getProject(const SgNode * node);

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
  NodeType::traverseMemoryPoolNodes(my_traversal);
  return my_traversal.resultlist;
}


/*! \brief top-down traversal from current node to find the main() function declaration
*/
ROSE_DLL_API SgFunctionDeclaration* findMain(SgNode* currentNode);

//! Find the last declaration statement within a scope (if any). This is often useful to decide where to insert another variable declaration statement. Pragma declarations are not treated as a declaration by default in this context. 
SgStatement* findLastDeclarationStatement(SgScopeStatement * scope, bool includePragma = false);

          //midend/programTransformation/partialRedundancyElimination/pre.h
//! Find referenced symbols within an expression
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

//! Collect all variable references in a subtree
void collectVarRefs(SgLocatedNode* root, std::vector<SgVarRefExp* >& result);

  //! Topdown traverse a subtree from root to find the first declaration given its name, scope (optional, can be NULL), and defining or nondefining flag.
template <typename T>
T* findDeclarationStatement(SgNode* root, std::string name, SgScopeStatement* scope, bool isDefining)
   {
     bool found = false;

#if 0
     printf ("In findDeclarationStatement(): root       = %p \n",root);
     printf ("In findDeclarationStatement(): name       = %s \n",name.c_str());
     printf ("In findDeclarationStatement(): scope      = %p \n",scope);
     printf ("In findDeclarationStatement(): isDefining = %s \n",isDefining ? "true" : "false");
#endif

  // Do we really want a NULL pointer to be acceptable input to this function? 
  // Maybe we should have an assertion that it is non-null?
     if (!root) return NULL;

     T* decl = dynamic_cast<T*>(root);

#if 0
     printf ("In findDeclarationStatement(): decl       = %p \n",decl);
#endif

     if (decl != NULL)
        {
          if (scope)
             {
               if ((decl->get_scope() == scope) && (decl->search_for_symbol_from_symbol_table()->get_name() == name))
                  { 
                    found = true;
                  }
             }
            else // Liao 2/9/2010. We should allow NULL scope
             {
#if 0
            // DQ (12/6/2016): Include this into the debugging code to aboid compiler warning about unused variable.
               SgSymbol* symbol = decl->search_for_symbol_from_symbol_table();
               printf ("In findDeclarationStatement(): decl->search_for_symbol_from_symbol_table() = %p \n",symbol);
               printf ("In findDeclarationStatement(): decl->search_for_symbol_from_symbol_table()->get_name() = %s \n",symbol->get_name().str());
#endif
               if (decl->search_for_symbol_from_symbol_table()->get_name() == name)
                  {
                    found = true;
                  }
             }
        }

     if (found)
        {
          if (isDefining)
             {
#if 0
               printf ("In findDeclarationStatement(): decl->get_firstNondefiningDeclaration() = %p \n",decl->get_firstNondefiningDeclaration());
               printf ("In findDeclarationStatement(): decl->get_definingDeclaration()         = %p \n",decl->get_definingDeclaration());
#endif
               ROSE_ASSERT (decl->get_definingDeclaration() != NULL);
#if 0
               printf ("In findDeclarationStatement(): returing decl->get_definingDeclaration() = %p \n",decl->get_definingDeclaration());
#endif
               return dynamic_cast<T*> (decl->get_definingDeclaration()); 
             }
            else 
             {
#if 0
               printf ("In findDeclarationStatement(): returing decl = %p \n",decl);
#endif
               return decl;
             }
        }

     std::vector<SgNode*> children = root->get_traversalSuccessorContainer();

#if 0
     printf ("In findDeclarationStatement(): children.size() = %zu \n",children.size());
#endif

  // DQ (4/10/2016): Note that if we are searching for a function member that has it's defining 
  // declaration defined outside of the class then it will not be found in the child list.
     for (std::vector<SgNode*>::const_iterator i = children.begin(); i != children.end(); ++i)
        {
          T* target = findDeclarationStatement<T> (*i,name,scope,isDefining);

          if (target)
             {
               return target;
             }
        }

     return NULL;
   }
//! Topdown traverse a subtree from root to find the first function declaration matching the given name, scope (optional, can be NULL), and defining or nondefining flag. This is an instantiation of findDeclarationStatement<T>.
  SgFunctionDeclaration* findFunctionDeclaration(SgNode* root, std::string name, SgScopeStatement* scope, bool isDefining);

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


/** Find a node by type using upward traversal.
 *
 *  Traverse backward through a specified node's ancestors, starting with the node's parent and progressing to more distant
 *  ancestors, to find the first node matching the specified or derived type.  If @p includingSelf is true then the
 *  starting node, @p astNode, is returned if its type matches, otherwise the search starts at the parent of @p astNode.
 *
 *  For the purposes of this function, the parent (P) of an SgDeclarationStatement node (N) is considered to be the first
 *  non-defining declaration of N if N has both a defining declaration and a first non-defining declaration and the defining
 *  declaration is different than the first non-defining declaration.
 *
 *  If no ancestor of the requisite type of subtypes is found then this function returns a null pointer.
 *
 *  If @p astNode is the null pointer, then the return value is a null pointer. That is, if there is no node, then there cannot
 *  be an enclosing node of the specified type. */
template <typename NodeType>
NodeType* getEnclosingNode(const SgNode* astNode, const bool includingSelf = false)
   {
#define DEBUG_GET_ENCLOSING_NODE 0

#if 1
  // DQ (12/31/2019): This version does not detect a cycle that Robb's version detects in processing Cxx11_tests/test2016_23.C.
  // This will have to be investigated seperately from the issue I am working on currently.

  // DQ (10/20/2012): This is the older version of this implementation.  Until I am sure that
  // the newer version (below) is what we want to use I will resolve this conflict by keeping
  // the previous version in place.

     if (NULL == astNode)
        {
          return NULL;
        }

     if ( (includingSelf ) && (dynamic_cast<const NodeType*>(astNode)) )
        {
          return const_cast<NodeType*>(dynamic_cast<const NodeType*> (astNode));
        }

  // DQ (3/5/2012): Check for reference to self...
     ROSE_ASSERT(astNode->get_parent() != astNode);

     SgNode* parent = astNode->get_parent();

  // DQ (3/5/2012): Check for loops that will cause infinite loops.
     SgNode* previouslySeenParent = parent;
     bool foundCycle = false;
     int counter = 0;

#if DEBUG_GET_ENCLOSING_NODE
     printf ("In getEnclosingNode(): previouslySeenParent = %p = %s \n",previouslySeenParent,previouslySeenParent->class_name().c_str());
#endif

     while ( (foundCycle == false) && (parent != NULL) && (!dynamic_cast<const NodeType*>(parent)) )
        {
          ROSE_ASSERT(parent->get_parent() != parent);

#if DEBUG_GET_ENCLOSING_NODE
          printf (" --- parent = %p = %s \n",parent,parent->class_name().c_str());
          printf (" --- --- parent->get_parent() = %p = %s \n",parent->get_parent(),parent->get_parent()->class_name().c_str());
#endif

#if 1
       // DQ (1/8/2020): ROSE-82 (on RZ) This limit needs to be larger and increasing it to 500 was enough 
       // for a specific code with a long chain of if-then-else nesting, So to make this sufficent for more
       // general code we have increased the lomit to 100,000.  Note that 50 was not enough for real code, 
       // but was enough for our regression tests.
       // DQ (12/30/2019): This is added to support detection of infinite loops over parent pointers.
       // if (counter >= 500)
          if (counter >= 100000)
             {
               printf ("Exiting: In getEnclosingNode(): loop limit exceeded: counter = %d \n",counter);
               ROSE_ASSERT(false);
             }
#endif
          parent = parent->get_parent();

       // DQ (3/5/2012): Check for loops that will cause infinite loops.
       // ROSE_ASSERT(parent != previouslySeenParent);
          if (parent == previouslySeenParent)
             {
               foundCycle = true;
             }
          counter++;

        }

#if DEBUG_GET_ENCLOSING_NODE
     printf ("previouslySeenParent = %p = %s \n",previouslySeenParent,previouslySeenParent->class_name().c_str());
#endif

     parent = previouslySeenParent;

     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(parent);
     if (declarationStatement != NULL)
        {
#if 0
          printf ("Found a SgDeclarationStatement \n");
#endif
          SgDeclarationStatement* definingDeclaration         = declarationStatement->get_definingDeclaration();
          SgDeclarationStatement* firstNondefiningDeclaration = declarationStatement->get_firstNondefiningDeclaration();

#if 0
          printf (" --- declarationStatement         = %p \n",declarationStatement);
          printf (" --- definingDeclaration          = %p \n",definingDeclaration);
          if (definingDeclaration != NULL && definingDeclaration->get_parent() != NULL)
               printf (" --- definingDeclaration ->get_parent()         = %p = %s \n",definingDeclaration->get_parent(),definingDeclaration->get_parent()->class_name().c_str());
          printf (" --- firstNondefiningDeclaration  = %p \n",firstNondefiningDeclaration);
          if (firstNondefiningDeclaration != NULL && firstNondefiningDeclaration->get_parent() != NULL)
               printf (" --- firstNondefiningDeclaration ->get_parent() = %p = %s \n",firstNondefiningDeclaration->get_parent(),firstNondefiningDeclaration->get_parent()->class_name().c_str());
#endif
          if (definingDeclaration != NULL && declarationStatement != firstNondefiningDeclaration)
             {
#if 0
               printf ("Found a nondefining declaration so use the non-defining declaration instead \n");
#endif
            // DQ (10/19/2012): Use the defining declaration instead.
            // parent = firstNondefiningDeclaration;
               parent = definingDeclaration;
             }
        }

#if 0
     printf ("reset: previouslySeenParent = %p = %s \n",previouslySeenParent,previouslySeenParent->class_name().c_str());
#endif

  // DQ (10/19/2012): This branch is just to document the cycle that was previously detected, it is for 
  // debugging only. Thus it ony make sense for it to be executed when "(foundCycle == true)".  However, 
  // this will have to be revisited later since it appears clear that it is a problem for the binary analysis 
  // work when it is visited for this case.  Since the cycle is detected, but there is no assertion on the
  // cycle, we don't exit when a cycle is identified (which is the point of the code below). 
  // Note also that I have fixed the code (above and below) to only chase pointers through defining 
  // declarations (where they exist), this is important since non-defining declarations can be almost
  // anywhere (and thus chasing them can make it appear that there are cycles where there are none 
  // (I think); test2012_234.C demonstrates an example of this.
  // DQ (10/9/2012): Robb has suggested this change to fix the binary analysis work.
  // if (foundCycle == true)
     if (foundCycle == false)
        {


          while ( (parent != NULL) && (!dynamic_cast<const NodeType*>(parent)) )
             {
               ROSE_ASSERT(parent->get_parent() != parent);
#if 0
               printf ("In getEnclosingNode() (2nd try): parent = %p = %s \n",parent,parent->class_name().c_str());
               if (parent->get_file_info() != NULL)
                    parent->get_file_info()->display("In getEnclosingNode() (2nd try): debug");
#endif
               SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(parent);
               if (declarationStatement != NULL)
                  {
#if DEBUG_GET_ENCLOSING_NODE
                    printf ("Found a SgDeclarationStatement \n");
#endif
                    SgDeclarationStatement* definingDeclaration         = declarationStatement->get_definingDeclaration();
                    SgDeclarationStatement* firstNondefiningDeclaration = declarationStatement->get_firstNondefiningDeclaration();
#if 0
                    printf (" --- declarationStatement                       = %p = %s \n",declarationStatement,(declarationStatement != NULL) ? declarationStatement->class_name().c_str() : "null");

                    printf (" --- definingDeclaration                        = %p \n",definingDeclaration);
                    if (definingDeclaration != NULL && definingDeclaration->get_parent() != NULL)
                         printf (" --- definingDeclaration ->get_parent()         = %p = %s \n",definingDeclaration->get_parent(),definingDeclaration->get_parent()->class_name().c_str());

                    printf (" --- firstNondefiningDeclaration                = %p \n",firstNondefiningDeclaration);
                    if (firstNondefiningDeclaration != NULL && firstNondefiningDeclaration->get_parent() != NULL)
                         printf (" --- firstNondefiningDeclaration ->get_parent() = %p = %s \n",firstNondefiningDeclaration->get_parent(),firstNondefiningDeclaration->get_parent()->class_name().c_str());
#endif
                    if (definingDeclaration != NULL && declarationStatement != firstNondefiningDeclaration)
                       {
#if 0
                         printf ("Found a nondefining declaration so use the firstNondefining declaration instead \n");
#endif
                      // DQ (10/19/2012): Use the defining declaration instead.
                      // parent = firstNondefiningDeclaration;
                         parent = definingDeclaration;
                       }
                  }

               parent = parent->get_parent();

#if 1
            // DQ (3/5/2012): Check for loops that will cause infinite loops.
               ROSE_ASSERT(parent != previouslySeenParent);
#else
               printf ("WARNING::WARNING::WARNING commented out assertion for parent != previouslySeenParent \n");
               if (parent == previouslySeenParent)
                    break;
#endif
             }
        }

     return const_cast<NodeType*>(dynamic_cast<const NodeType*> (parent));
#else
  // DQ (10/20/2012): Using Robb's newer version with my modification to use the definingDeclaration rather than firstNondefiningDeclaration (below).

       // Find the parent of specified type, but watch out for cycles in the ancestry (which would cause an infinite loop).
       // Cast away const because isSg* functions aren't defined for const node pointers; and our return is not const.
       SgNode *node = const_cast<SgNode*>(!astNode || includingSelf ? astNode : astNode->get_parent());
       std::set<const SgNode*> seen; // nodes we've seen, in order to detect cycles
       while (node) {
           if (NodeType *found = dynamic_cast<NodeType*>(node))
               return found;

        // FIXME: Cycle detection could be moved elsewhere so we don't need to do it on every call. [RPM 2012-10-09]
        // DQ (12/30/2019): Provide more detail in error message.
           if (seen.insert(node).second == false)
             {
               printf ("Error: node is already in set and defines a cycle: node = %p = %s \n",node,node->class_name().c_str());
               std::set<const SgNode*>::const_iterator i = seen.begin();
               while (i != seen.end())
                 {
                   const SgNode* element = *i;
                   printf (" --- seen element: element = %p = %s \n",element,element->class_name().c_str()); 
                   i++;
                 }

               printf ("Exiting after error! \n");
               ROSE_ASSERT(false);
             }
           // ROSE_ASSERT(seen.insert(node).second);

           // Traverse to parent (declaration statements are a special case)
           if (SgDeclarationStatement *declarationStatement = isSgDeclarationStatement(node)) {
               SgDeclarationStatement *definingDeclaration = declarationStatement->get_definingDeclaration();
               SgDeclarationStatement *firstNondefiningDeclaration = declarationStatement->get_firstNondefiningDeclaration();
               if (definingDeclaration && firstNondefiningDeclaration && declarationStatement != firstNondefiningDeclaration) {
                // DQ (10/19/2012): Use the defining declaration instead.
                // node = firstNondefiningDeclaration;
                   node = definingDeclaration;
               }
           } else {
               node = node->get_parent();
           }
       }
       return NULL;
#endif
   }

  //! Find enclosing source file node 
  ROSE_DLL_API SgSourceFile* getEnclosingSourceFile(SgNode* n, const bool includingSelf=false);

  //! Get the closest scope from astNode. Return astNode if it is already a scope.
  ROSE_DLL_API SgScopeStatement* getScope(const SgNode* astNode);

  //! Get the enclosing scope from a node n 
  ROSE_DLL_API SgScopeStatement* getEnclosingScope(SgNode* n, const bool includingSelf=false);

  //! Traverse back through a node's parents to find the enclosing global scope
  ROSE_DLL_API SgGlobal* getGlobalScope( const SgNode* astNode);


  //! Find the function definition
  ROSE_DLL_API SgFunctionDefinition* getEnclosingProcedure(SgNode* n, const bool includingSelf=false);

  ROSE_DLL_API SgFunctionDefinition* getEnclosingFunctionDefinition(SgNode* astNode, const bool includingSelf=false);

  //! Find the closest enclosing statement, including the given node
  ROSE_DLL_API SgStatement* getEnclosingStatement(SgNode* n);

  //! Find the closest switch outside a given statement (normally used for case and default statements)
  ROSE_DLL_API SgSwitchStatement* findEnclosingSwitch(SgStatement* s);

  //! Find enclosing OpenMP clause body statement from s. If s is already one, return it directly. 
  ROSE_DLL_API SgOmpClauseBodyStatement* findEnclosingOmpClauseBodyStatement(SgStatement* s);

  //! Find the closest loop outside the given statement; if fortranLabel is not empty, the Fortran label of the loop must be equal to it
  ROSE_DLL_API SgScopeStatement* findEnclosingLoop(SgStatement* s, const std::string& fortranLabel = "", bool stopOnSwitches = false);

  //! Find the enclosing function declaration, including its derived instances like isSgProcedureHeaderStatement, isSgProgramHeaderStatement, and isSgMemberFunctionDeclaration.
  ROSE_DLL_API SgFunctionDeclaration * getEnclosingFunctionDeclaration (SgNode * astNode, const bool includingSelf=false);
   //roseSupport/utility_functions.h
  //! get the SgFile node from current node
  ROSE_DLL_API SgFile* getEnclosingFileNode (SgNode* astNode );

  //! Get the initializer containing an expression if it is within an initializer.
  ROSE_DLL_API SgInitializer* getInitializerOfExpression(SgExpression* n);

  //! Get the closest class definition enclosing the specified AST node,
  ROSE_DLL_API SgClassDefinition* getEnclosingClassDefinition(SgNode* astnode, const bool includingSelf=false);

  //! Get the closest class declaration enclosing the specified AST node,
  ROSE_DLL_API SgClassDeclaration* getEnclosingClassDeclaration( SgNode* astNode );

  // DQ (2/7/2019): Adding support for name qualification of variable references associated with SgPointerMemberType function parameters.
  //! Get the enclosing SgExprListExp (used as part of function argument index evaluation in subexpressions).
  ROSE_DLL_API SgExprListExp* getEnclosingExprListExp(SgNode* astNode, const bool includingSelf = false);

  // DQ (2/7/2019): Need a function to return when an expression is in an expression subtree.
  // This is part of index evaluation ofr expressions in function argument lists, but likely usefule elsewhere as well.
  ROSE_DLL_API bool isInSubTree(SgExpression* subtree, SgExpression* exp);

  // DQ (2/7/2019): Need a function to return the SgFunctionDeclaration from a SgFunctionCallExp.
  ROSE_DLL_API SgFunctionDeclaration* getFunctionDeclaration ( SgFunctionCallExp* functionCallExp );

  // DQ (2/17/2019): Generalizing this support for SgVarRefExp and SgMemberFunctionRefExp nodes.
  // DQ (2/8/2019): Adding support for detecting when to use added name qualification for pointer-to-member expressions.
  ROSE_DLL_API bool isDataMemberReference(SgVarRefExp* varRefExp);
  // ROSE_DLL_API bool isAddressTaken(SgVarRefExp* varRefExp);
  ROSE_DLL_API bool isAddressTaken(SgExpression* refExp);

  // DQ (2/17/2019): Adding support for detecting when to use added name qualification for membr function references.
  ROSE_DLL_API bool isMemberFunctionMemberReference(SgMemberFunctionRefExp* memberFunctionRefExp);

  // DQ (2/15/2019): Adding support for detecting which class a member reference is being made from.
  // ROSE_DLL_API SgClassType* getClassTypeForDataMemberReference(SgVarRefExp* varRefExp);
  // ROSE_DLL_API std::list<SgClassType*> getClassTypeChainForDataMemberReference(SgVarRefExp* varRefExp);
  ROSE_DLL_API std::list<SgClassType*> getClassTypeChainForMemberReference(SgExpression* refExp);

  ROSE_DLL_API std::set<SgNode*> getFrontendSpecificNodes();

  // DQ (2/17/2019): Display the shared nodes in the AST for debugging.
  ROSE_DLL_API void outputSharedNodes( SgNode* node );


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
  ROSE_DLL_API SgGlobal * getFirstGlobalScope(SgProject *project);

  /*!
        \brief get the last statement within a scope, return NULL if it does not exit
  */
  ROSE_DLL_API SgStatement* getLastStatement(SgScopeStatement *scope);

  //! Get the first statement within a scope, return NULL if it does not exist. Skip compiler-generated statement by default. Count transformation-generated ones, but excluding those which are not to be outputted in unparsers.
  ROSE_DLL_API SgStatement* getFirstStatement(SgScopeStatement *scope,bool includingCompilerGenerated=false);
    //!Find the first defining function declaration statement in a scope
  ROSE_DLL_API SgFunctionDeclaration* findFirstDefiningFunctionDecl(SgScopeStatement* scope);

//! Get next statement within the same scope of current statement
  ROSE_DLL_API SgStatement* getNextStatement(SgStatement * currentStmt);

//! Get previous statement of the current statement. It may return a previous statement of a parent scope by default (climbOutScope is true), otherwise only a previous statement of the same scope is returned.  
  ROSE_DLL_API SgStatement* getPreviousStatement(SgStatement * currentStmt, bool climbOutScope = true);
#if 0 //TODO
  // preorder traversal from current SgNode till find next SgNode of type V_SgXXX
  SgNode* getNextSgNode( const SgNode* currentNode, VariantT=V_SgNode);
#endif

  // DQ (11/15/2018): Adding support for traversals over the include file tree.
  //! return path prefix for subtree of include files. 
  void listHeaderFiles ( SgIncludeFile* includeFile );


//@}

//------------------------------------------------------------------------
//@{
/*! @name AST Comparison
  \brief Compare AST nodes, subtree, etc
*/
  //! Check if a SgIntVal node has a given value
 ROSE_DLL_API bool isEqualToIntConst(SgExpression* e, int value);

 //! Check if two function declarations refer to the same one. Two function declarations are the same when they are a) identical, b) same name in C c) same qualified named and mangled name in C++. A nondefining (prototype) declaration and a defining declaration of a same function are treated as the same.
 /*!
  * There is a similar function bool compareFunctionDeclarations(SgFunctionDeclaration *f1, SgFunctionDeclaration *f2) from Classhierarchy.C
  */
 ROSE_DLL_API bool isSameFunction(SgFunctionDeclaration* func1, SgFunctionDeclaration* func2);

 //! Check if a statement is the last statement within its closed scope
 ROSE_DLL_API bool isLastStatement(SgStatement* stmt);

//@}

//------------------------------------------------------------------------
//@{
/*! @name AST insert, removal, and replacement
  \brief Add, remove,and replace AST

  scope->append_statement(), exprListExp->append_expression() etc. are not enough to handle side effect of parent pointers, symbol tables, preprocessing info, defining/nondefining pointers etc.
*/

// DQ (2/24/2009): Simple function to delete an AST subtree (used in outlining).
//! Function to delete AST subtree's nodes only, users must take care of any dangling pointers, symbols or types that result.
ROSE_DLL_API void deleteAST(SgNode* node);

//! Special purpose function for deleting AST expression tress containing valid original expression trees in constant folded expressions (for internal use only).
ROSE_DLL_API void deleteExpressionTreeWithOriginalExpressionSubtrees(SgNode* root);

// DQ (2/25/2009): Added new function to support outliner.
//! Move statements in first block to the second block (preserves order and rebuilds the symbol table).
ROSE_DLL_API void moveStatementsBetweenBlocks ( SgBasicBlock* sourceBlock, SgBasicBlock* targetBlock );

//! Move a variable declaration to a new scope, handle symbol, special scopes like For loop, etc.
ROSE_DLL_API void moveVariableDeclaration(SgVariableDeclaration* decl, SgScopeStatement* target_scope);
//! Append a statement to the end of the current scope, handle side effect of appending statements, e.g. preprocessing info, defining/nondefining pointers etc.
ROSE_DLL_API void appendStatement(SgStatement *stmt, SgScopeStatement* scope=NULL);

//! Append a statement to the end of SgForInitStatement
ROSE_DLL_API void appendStatement(SgStatement *stmt, SgForInitStatement* for_init_stmt);

//! Append a list of statements to the end of the current scope, handle side effect of appending statements, e.g. preprocessing info, defining/nondefining pointers etc.
ROSE_DLL_API void appendStatementList(const std::vector<SgStatement*>& stmt, SgScopeStatement* scope=NULL);

// DQ (2/6/2009): Added function to support outlining into separate file.
//! Append a copy ('decl') of a function ('original_statement') into a 'scope', include any referenced declarations required if the scope is within a compiler generated file. All referenced declarations, including those from headers, are inserted if excludeHeaderFiles is set to true (the new file will not have any headers).
ROSE_DLL_API void appendStatementWithDependentDeclaration( SgDeclarationStatement* decl, SgGlobal* scope, SgStatement* original_statement, bool excludeHeaderFiles );

//! Prepend a statement to the beginning of the current scope, handling side
//! effects as appropriate
ROSE_DLL_API void prependStatement(SgStatement *stmt, SgScopeStatement* scope=NULL);

//! Prepend a statement to the beginning of SgForInitStatement
ROSE_DLL_API void prependStatement(SgStatement *stmt, SgForInitStatement* for_init_stmt);

//! prepend a list of statements to the beginning of the current scope,
//! handling side effects as appropriate
ROSE_DLL_API void prependStatementList(const std::vector<SgStatement*>& stmt, SgScopeStatement* scope=NULL);

//! Check if a scope statement has a simple children statement list
//! so insert additional statements under the scope is straightforward and unambiguous .
//! for example, SgBasicBlock has a simple statement list while IfStmt does not.
ROSE_DLL_API bool  hasSimpleChildrenList (SgScopeStatement* scope);

//! Insert a statement before or after the target statement within the target's scope. Move around preprocessing info automatically
ROSE_DLL_API void insertStatement(SgStatement *targetStmt, SgStatement* newStmt, bool insertBefore= true, bool autoMovePreprocessingInfo = true);

//! Insert a list of statements before or after the target statement within the
//target's scope
ROSE_DLL_API void insertStatementList(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmts, bool insertBefore= true);

//! Insert a statement before a target statement
ROSE_DLL_API void insertStatementBefore(SgStatement *targetStmt, SgStatement* newStmt, bool autoMovePreprocessingInfo = true);

//! Insert a list of statements before a target statement
ROSE_DLL_API void insertStatementListBefore(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmts);

//! Insert a statement after a target statement, Move around preprocessing info automatically by default
ROSE_DLL_API void insertStatementAfter(SgStatement *targetStmt, SgStatement* newStmt, bool autoMovePreprocessingInfo = true);

//! Insert a list of statements after a target statement
ROSE_DLL_API void insertStatementListAfter(SgStatement *targetStmt, const std::vector<SgStatement*>& newStmt);

//! Insert a statement after the last declaration within a scope. The statement will be prepended to the scope if there is no declaration statement found
ROSE_DLL_API void insertStatementAfterLastDeclaration(SgStatement* stmt, SgScopeStatement* scope);

//! Insert a list of statements after the last declaration within a scope. The statement will be prepended to the scope if there is no declaration statement found
ROSE_DLL_API void insertStatementAfterLastDeclaration(std::vector<SgStatement*> stmt_list, SgScopeStatement* scope);

//! Insert a statement before the first non-declaration statement in a scope.  If the scope has no non-declaration statements
//  then the statement is inserted at the end of the scope.
ROSE_DLL_API void insertStatementBeforeFirstNonDeclaration(SgStatement *newStmt, SgScopeStatement *scope,
                                                           bool movePreprocessingInfo=true);

//! Insert statements before the first non-declaration statement in a scope.  If the scope has no non-declaration statements
//then the new statements are inserted at the end of the scope.
ROSE_DLL_API void insertStatementListBeforeFirstNonDeclaration(const std::vector<SgStatement*> &newStmts, SgScopeStatement *scope);

// DQ (11/21/2018): We need to sometimes insert something after the last statement of the collection from rose_edg_required_macros_and_functions.h.
ROSE_DLL_API SgStatement* lastFrontEndSpecificStatement( SgGlobal* globalScope );

//! Remove a statement from its attach point of the AST. Automatically keep its associated preprocessing information at the original place after the removal. The statement is still in memory and it is up to the users to decide if the removed one will be inserted somewhere else or released from memory (deleteAST()).
ROSE_DLL_API void removeStatement(SgStatement* stmt, bool autoRelocatePreprocessingInfo = true);

//! Deep delete a sub AST tree. It uses postorder traversal to delete each child node. Users must take care of any dangling pointers, symbols or types that result. This is identical to deleteAST()
ROSE_DLL_API void deepDelete(SgNode* root);

//! Replace a statement with another. Move preprocessing information from oldStmt to newStmt if requested.
ROSE_DLL_API void replaceStatement(SgStatement* oldStmt, SgStatement* newStmt, bool movePreprocessinInfo = false);

//! Replace an anchor node with a specified pattern subtree with optional SgVariantExpression. All SgVariantExpression in the pattern will be replaced with copies of the anchor node.
ROSE_DLL_API SgNode* replaceWithPattern (SgNode * anchor, SgNode* new_pattern);

//! Replace all variable references to an old symbol in a scope to being references to a new symbol.
// Essentially replace variable a with b. 
ROSE_DLL_API void replaceVariableReferences(SgVariableSymbol* old_sym, SgVariableSymbol* new_sym, SgScopeStatement * scope );

// DQ (11/12/2018): Adding test to avoid issues that we can't test for in the unparsing of header files using the token based unparsing.
//! If header file unparsing and token-based unparsing are used, then some statements in header files 
//! used with the same name and different include syntax can't be transformed. This is currently because 
//! there is no way to generally test the resulting transformed code generated by ROSE.
ROSE_DLL_API bool statementCanBeTransformed(SgStatement* stmt);


/** Given an expression, generates a temporary variable whose initializer optionally evaluates
* that expression. Then, the var reference expression returned can be used instead of the original
* expression. The temporary variable created can be reassigned to the expression by the returned SgAssignOp;
* this can be used when the expression the variable represents needs to be evaluated. NOTE: This handles
* reference types correctly by using pointer types for the temporary.
* @param expression Expression which will be replaced by a variable
* @param scope scope in which the temporary variable will be generated
* @param reEvaluate an assignment op to reevaluate the expression. Leave NULL if not needed
* @return declaration of the temporary variable, and a a variable reference expression to use instead of
* the original expression. */
std::pair<SgVariableDeclaration*, SgExpression* > createTempVariableForExpression(SgExpression* expression,
        SgScopeStatement* scope, bool initializeInDeclaration, SgAssignOp** reEvaluate = NULL);

/*  This function creates a temporary variable for a given expression in the given scope
   This is different from SageInterface::createTempVariableForExpression in that it does not
   try to be smart to create pointers to reference types and so on. The tempt is initialized to expression.
   The caller is responsible for setting the parent of SgVariableDeclaration since buildVariableDeclaration
   may not set_parent() when the scope stack is empty. See programTransformation/extractFunctionArgumentsNormalization/ExtractFunctionArguments.C for sample usage.
   @param expression Expression which will be replaced by a variable
   @param scope scope in which the temporary variable will be generated
*/
    
std::pair<SgVariableDeclaration*, SgExpression*> createTempVariableAndReferenceForExpression
    (SgExpression* expression, SgScopeStatement* scope);
    
//! Append an argument to SgFunctionParameterList, transparently set parent,scope, and symbols for arguments when possible
/*! We recommend to build SgFunctionParameterList before building a function declaration
 However, it is still allowed to append new arguments for existing function declarations.
 \todo function type , function symbol also need attention.
*/
ROSE_DLL_API SgVariableSymbol* appendArg(SgFunctionParameterList *, SgInitializedName*);
//!Prepend an argument to SgFunctionParameterList
ROSE_DLL_API SgVariableSymbol* prependArg(SgFunctionParameterList *, SgInitializedName*);

//! Append an expression to a SgExprListExp, set the parent pointer also
ROSE_DLL_API void appendExpression(SgExprListExp *, SgExpression*);

//! Append an expression list to a SgExprListExp, set the parent pointers also
ROSE_DLL_API void appendExpressionList(SgExprListExp *, const std::vector<SgExpression*>&);

//! Set parameter list for a function declaration, considering existing parameter list etc.
template <class actualFunction> 
void setParameterList(actualFunction *func,SgFunctionParameterList *paralist) {

  // TODO consider the difference between C++ and Fortran
  // fixup the scope of arguments,no symbols for nondefining function declaration's arguments

  // DQ (11/25/2011): templated function so that we can handle both 
  // SgFunctionDeclaration and SgTemplateFunctionDeclaration (and their associated member 
  // function derived classes).

     ROSE_ASSERT(func != NULL);
     ROSE_ASSERT(paralist != NULL);

#if 0
  // At this point we don't have cerr and endl defined, so comment this code out.
  // Warn to users if a paralist is being shared
     if (paralist->get_parent() !=NULL)
        {
          cerr << "Waring! Setting a used SgFunctionParameterList to function: "
               << (func->get_name()).getString()<<endl
               << " Sharing parameter lists can corrupt symbol tables!"<<endl
               << " Please use deepCopy() to get an exclusive parameter list for each function declaration!"<<endl;
       // ROSE_ASSERT(false);
        }
#endif

  // Liao,2/5/2008  constructor of SgFunctionDeclaration will automatically generate SgFunctionParameterList, so be cautious when set new paralist!!
     if (func->get_parameterList() != NULL)
        {
          if (func->get_parameterList() != paralist)
             {
               delete func->get_parameterList();
             }
        }

     func->set_parameterList(paralist);
     paralist->set_parent(func);

  // DQ (5/15/2012): Need to set the declptr in each SgInitializedName IR node.
  // This is needed to support the AST Copy mechanism (at least). The files: test2005_150.C, 
  // test2012_81.C and testcode2012_82.C demonstrate this problem.
     SgInitializedNamePtrList & args = paralist->get_args();
     for (SgInitializedNamePtrList::iterator i = args.begin(); i != args.end(); i++)
        {
          (*i)->set_declptr(func);
        }
   }

//! Set a pragma of a pragma declaration. handle memory release for preexisting pragma, and set parent pointer.
ROSE_DLL_API void setPragma(SgPragmaDeclaration* decl, SgPragma *pragma);

  //! Replace an expression with another, used for variable reference substitution and others. the old expression can be deleted (default case)  or kept.
ROSE_DLL_API void replaceExpression(SgExpression* oldExp, SgExpression* newExp, bool keepOldExp=false);

//! Replace a given expression with a list of statements produced by a generator
ROSE_DLL_API void replaceExpressionWithStatement(SgExpression* from,
                                    SageInterface::StatementGenerator* to);
//! Similar to replaceExpressionWithStatement, but with more restrictions.
//! Assumptions: from is not within the test of a loop or ifStmt,  not currently traversing from or the statement it is in
ROSE_DLL_API void replaceSubexpressionWithStatement(SgExpression* from,
                                      SageInterface::StatementGenerator* to);

//! Set operands for expressions with single operand, such as unary expressions. handle file info, lvalue, pointer downcasting, parent pointer etc.
ROSE_DLL_API void setOperand(SgExpression* target, SgExpression* operand);

//!set left hand operand for binary expressions, transparently downcasting target expressions when necessary
ROSE_DLL_API void setLhsOperand(SgExpression* target, SgExpression* lhs);

//!set left hand operand for binary expression
ROSE_DLL_API void setRhsOperand(SgExpression* target, SgExpression* rhs);

//! Set original expression trees to NULL for SgValueExp or SgCastExp expressions, so you can change the value and have it unparsed correctly.
ROSE_DLL_API void removeAllOriginalExpressionTrees(SgNode* top);

// DQ (1/25/2010): Added support for directories
//! Move file to be generated in a subdirectory (will be generated by the unparser).
ROSE_DLL_API void moveToSubdirectory ( std::string directoryName, SgFile* file );

//! Supporting function to comment relocation in insertStatement() and removeStatement().
ROSE_DLL_API SgStatement* findSurroundingStatementFromSameFile(SgStatement* targetStmt, bool & surroundingStatementPreceedsTargetStatement);

//! Relocate comments and CPP directives from one statement to another.
ROSE_DLL_API void moveCommentsToNewStatement(SgStatement* sourceStatement, const std::vector<int> & indexList, SgStatement* targetStatement, bool surroundingStatementPreceedsTargetStatement);

// DQ (7/19/2015): This is required to support general unparsing of template instantations for the GNU g++
// compiler which does not permit name qualification to be used to support the expression of the namespace
// where a template instantiatoon would be places.  Such name qualification would also sometimes require
// global qualification which is also not allowed by the GNU g++ compiler.  These issues appear to be 
// specific to the GNU compiler versions, at least versions 4.4 through 4.8.
//! Relocate the declaration to be explicitly represented in its associated namespace (required for some backend compilers to process template instantiations).
ROSE_DLL_API void moveDeclarationToAssociatedNamespace ( SgDeclarationStatement* declarationStatement );

ROSE_DLL_API bool isTemplateInstantiationNode(SgNode* node);

ROSE_DLL_API void wrapAllTemplateInstantiationsInAssociatedNamespaces(SgProject* root);

// DQ (12/1/2015): Adding support for fixup internal data struuctures that have references to statements (e.g. macro expansions).
ROSE_DLL_API void resetInternalMapsForTargetStatement(SgStatement* sourceStatement);

// DQ (6/7/2019): Add support for transforming function definitions to function prototypes in a subtree.
// We might have to make this specific to a file (only traversing the functions in that file).
/*!\brief XXX 
 * This function operates on the new file used to support outlined function definitions.
 * We use a copy of the file where the code will be outlined FROM, so that if there are references to
 * declarations in the outlined code we can support the outpiled code with those references.  This
 * approach has the added advantage of also supporting the same include file tree as the original 
 * file where the outlined code is being taken from.
 */
ROSE_DLL_API void convertFunctionDefinitionsToFunctionPrototypes(SgNode* node);

// DQ (11/10/2019): Lower level support for convertFunctionDefinitionsToFunctionPrototypes().
ROSE_DLL_API void replaceDefiningFunctionDeclarationWithFunctionPrototype ( SgFunctionDeclaration* functionDeclaration );
ROSE_DLL_API std::vector<SgFunctionDeclaration*> generateFunctionDefinitionsList(SgNode* node);


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
ROSE_DLL_API int fixVariableReferences(SgNode* root,  bool cleanUnusedSymbol=true);

//!Patch up symbol, scope, and parent information when a SgVariableDeclaration's scope is known.
/*!
It is possible to build a variable declaration without knowing its scope information during bottom-up construction of AST, though top-down construction is recommended in general.
In this case, we have to patch up symbol table, scope and parent information when the scope is known. This function is usually used internally within appendStatment(), insertStatement().
*/
ROSE_DLL_API void fixVariableDeclaration(SgVariableDeclaration* varDecl, SgScopeStatement* scope);

//! Fix symbols, parent and scope pointers. Used internally within appendStatment(), insertStatement() etc when a struct declaration was built without knowing its target scope.
ROSE_DLL_API void fixStructDeclaration(SgClassDeclaration* structDecl, SgScopeStatement* scope);
//! Fix symbols, parent and scope pointers. Used internally within appendStatment(), insertStatement() etc when a class declaration was built without knowing its target scope.
ROSE_DLL_API void fixClassDeclaration(SgClassDeclaration* classDecl, SgScopeStatement* scope);

//! Fix symbols, parent and scope pointers. Used internally within appendStatment(), insertStatement() etc when a namespace declaration was built without knowing its target scope.
ROSE_DLL_API void fixNamespaceDeclaration(SgNamespaceDeclarationStatement* structDecl, SgScopeStatement* scope);


//! Fix symbol table for SgLabelStatement. Used Internally when the label is built without knowing its target scope. Both parameters cannot be NULL.
ROSE_DLL_API void fixLabelStatement(SgLabelStatement* label_stmt, SgScopeStatement* scope);

//! Set a numerical label for a Fortran statement. The statement should have a enclosing function definition already. SgLabelSymbol and SgLabelRefExp are created transparently as needed.
ROSE_DLL_API void setFortranNumericLabel(SgStatement* stmt, int label_value,
                                         SgLabelSymbol::label_type_enum label_type=SgLabelSymbol::e_start_label_type,
                                         SgScopeStatement* label_scope=NULL);

//! Suggest next usable (non-conflicting) numeric label value for a Fortran function definition scope
ROSE_DLL_API int  suggestNextNumericLabel(SgFunctionDefinition* func_def);

//! Fix the symbol table and set scope (only if scope in declaration is not already set).
ROSE_DLL_API void fixFunctionDeclaration(SgFunctionDeclaration* stmt, SgScopeStatement* scope);

//! Fix the symbol table and set scope (only if scope in declaration is not already set).
ROSE_DLL_API void fixTemplateDeclaration(SgTemplateDeclaration* stmt, SgScopeStatement* scope);

//! A wrapper containing fixes (fixVariableDeclaration(),fixStructDeclaration(), fixLabelStatement(), etc) for all kinds statements. Should be used before attaching the statement into AST.
ROSE_DLL_API void fixStatement(SgStatement* stmt, SgScopeStatement* scope);

// DQ (6/11/2015): This reports the statements that are marked as transformed (used to debug the token-based unparsing).
//! This collects the statements that are marked as transformed (useful in debugging).
ROSE_DLL_API std::set<SgStatement*> collectTransformedStatements( SgNode* node );

//! This collects the statements that are marked as modified (a flag automatically set by all set_* generated functions) (useful in debugging).
ROSE_DLL_API std::set<SgStatement*> collectModifiedStatements( SgNode* node );

//! This collects the SgLocatedNodes that are marked as modified (a flag automatically set by all set_* generated functions) (useful in debugging).
ROSE_DLL_API std::set<SgLocatedNode*> collectModifiedLocatedNodes( SgNode* node );

// DQ (6/5/2019): Use the previously constructed set (above) to reset the IR nodes to be marked as isModified.
//! Use the set of IR nodes and set the isModified flag in each IR node to true.
ROSE_DLL_API void resetModifiedLocatedNodes(const std::set<SgLocatedNode*> & modifiedNodeSet);


// DQ (10/23/2018): Report nodes that are marked as modified.
ROSE_DLL_API void reportModifiedStatements(const std::string & label, SgNode* node);

// DQ (3/22/2019): Translate CPP directives from attached preprocessor information to CPP Directive Declaration IR nodes.
ROSE_DLL_API void translateToUseCppDeclarations( SgNode* n );

ROSE_DLL_API void translateScopeToUseCppDeclarations( SgScopeStatement* scope );

ROSE_DLL_API std::vector<SgC_PreprocessorDirectiveStatement*> translateStatementToUseCppDeclarations( SgStatement* statement, SgScopeStatement* scope);
ROSE_DLL_API void printOutComments ( SgLocatedNode* locatedNode );
ROSE_DLL_API bool skipTranslateToUseCppDeclaration( PreprocessingInfo* currentPreprocessingInfo );

// DQ (12/2/2019): Debugging support.
ROSE_DLL_API void outputFileIds( SgNode* node );


//@}

//! Update defining and nondefining links due to a newly introduced function declaration. Should be used after inserting the function into a scope.
/*! This function not only set the defining and nondefining links of the newly introduced
 *  function declaration inside a scope, but also update other same function declarations' links
 *  accordingly if there are any.
 *  Assumption: The function has already inserted/appended/prepended into the scope before calling this function.
 */
ROSE_DLL_API void updateDefiningNondefiningLinks(SgFunctionDeclaration* func, SgScopeStatement* scope);

//------------------------------------------------------------------------
//@{
/*! @name Advanced AST transformations, analyses, and optimizations
  \brief Some complex but commonly used AST transformations.
  */

//! Collect all read and write references within stmt, which can be a function, a scope statement, or a single statement. Note that a reference can be both read and written, like i++
ROSE_DLL_API bool
collectReadWriteRefs(SgStatement* stmt, std::vector<SgNode*>& readRefs, std::vector<SgNode*>& writeRefs, bool useCachedDefUse=false);

//!Collect unique variables which are read or written within a statement. Note that a variable can be both read and written. The statement can be either of a function, a scope, or a single line statement. For accesses to members of aggregate data, we return the coarse grain aggregate mem obj by default. 
ROSE_DLL_API bool collectReadWriteVariables(SgStatement* stmt, std::set<SgInitializedName*>& readVars, std::set<SgInitializedName*>& writeVars, bool coarseGrain=true);

//!Collect read only variables within a statement. The statement can be either of a function, a scope, or a single line statement. For accesses to members of aggregate data, we return the coarse grain aggregate mem obj by default.
ROSE_DLL_API void collectReadOnlyVariables(SgStatement* stmt, std::set<SgInitializedName*>& readOnlyVars, bool coarseGrain=true);

//!Collect read only variable symbols within a statement. The statement can be either of a function, a scope, or a single line statement. For accesses to members of aggregate data, we return the coarse grain aggregate mem obj by default.
ROSE_DLL_API void collectReadOnlySymbols(SgStatement* stmt, std::set<SgVariableSymbol*>& readOnlySymbols, bool coarseGrain=true);

//! Check if a variable reference is used by its address: including &a expression and foo(a) when type2 foo(Type& parameter) in C++
ROSE_DLL_API bool isUseByAddressVariableRef(SgVarRefExp* ref);

//! Collect variable references involving use by address: including &a expression and foo(a) when type2 foo(Type& parameter) in C++
ROSE_DLL_API void collectUseByAddressVariableRefs (const SgStatement* s, std::set<SgVarRefExp* >& varSetB);

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
//!Call liveness analysis on an entire project
ROSE_DLL_API LivenessAnalysis * call_liveness_analysis(SgProject* project, bool debug=false);

//!get liveIn and liveOut variables for a for loop from liveness analysis result liv.
ROSE_DLL_API void getLiveVariables(LivenessAnalysis * liv, SgForStatement* loop, std::set<SgInitializedName*>& liveIns, std::set<SgInitializedName*> & liveOuts);
#endif

//!Recognize and collect reduction variables and operations within a C/C++ loop, following OpenMP 3.0 specification for allowed reduction variable types and operation types.
ROSE_DLL_API void ReductionRecognition(SgForStatement* loop, std::set< std::pair <SgInitializedName*, OmpSupport::omp_construct_enum> > & results);

//! Constant folding an AST subtree rooted at 'r' (replacing its children with their constant values, if applicable). Please be advised that constant folding on floating point computation may decrease the accuracy of floating point computations!
/*! It is a wrapper function for ConstantFolding::constantFoldingOptimization(). Note that only r's children are replaced with their corresponding constant values, not the input SgNode r itself. You have to call this upon an expression's parent node if you want to fold the expression. */
ROSE_DLL_API void constantFolding(SgNode* r);

//!Instrument(Add a statement, often a function call) into a function right before the return points, handle multiple return statements (with duplicated statement s) and return expressions with side effects. Return the number of statements inserted.
/*! Useful when adding a runtime library call to terminate the runtime system right before the end of a program, especially for OpenMP and UPC runtime systems. Return with complex expressions with side effects are rewritten using an additional assignment statement.
 */
ROSE_DLL_API int instrumentEndOfFunction(SgFunctionDeclaration * func, SgStatement* s);

//! Remove jumps whose label is immediately after the jump.  Used to clean up inlined code fragments.
ROSE_DLL_API void removeJumpsToNextStatement(SgNode*);

//! Remove labels which are not targets of any goto statements
ROSE_DLL_API void removeUnusedLabels(SgNode* top);

//! Remove consecutive labels
ROSE_DLL_API void removeConsecutiveLabels(SgNode* top);

//! Merge a variable assignment statement into a matching variable declaration statement. Callers should make sure the merge is semantically correct (by not introducing compilation errors). This function simply does the merge transformation, without eligibility check.
/*!
 *  e.g.  int i;  i=10;  becomes int i=10;  the original i=10 will be deleted after the merge
 *  if success, return true, otherwise return false (e.g. variable declaration does not match or already has an initializer)
 *  The original assignment stmt will be removed by default
 *  This function is a bit ambiguous about the merge direction, to be phased out.
 */
ROSE_DLL_API bool mergeDeclarationAndAssignment (SgVariableDeclaration* decl, SgExprStatement* assign_stmt, bool removeAssignStmt = true);


//! Merge an assignment into its upstream declaration statement. Callers should make sure the merge is semantically correct.
ROSE_DLL_API bool mergeAssignmentWithDeclaration (SgExprStatement* assign_stmt, SgVariableDeclaration* decl, bool removeAssignStmt = true);

//! Merge a declaration statement into a matching followed variable assignment. Callers should make sure the merge is semantically correct (by not introducing compilation errors). This function simply does the merge transformation, without eligibility check.
/*! 
 *  e.g.  int i;  i=10;  becomes int i=10;  the original int i; will be deleted after the merge
 */
ROSE_DLL_API bool mergeDeclarationWithAssignment (SgVariableDeclaration* decl, SgExprStatement* assign_stmt);

//! Split a variable declaration with an rhs assignment into two statements: a declaration and an assignment. 
/*! Return the generated assignment statement, if any
 *  e.g.  int i =10;  becomes int i; i=10;  
 *  This can be seen as a normalization of declarations
 */
ROSE_DLL_API SgExprStatement* splitVariableDeclaration (SgVariableDeclaration* decl);

//! Split declarations within a scope into declarations and assignment statements, by default only top level declarations are considered. Return the number of declarations split.
ROSE_DLL_API int splitVariableDeclaration (SgScopeStatement* scope, bool topLevelOnly = true);

//! Replace an expression with a temporary variable and an assignment statement
/*!
 Add a new temporary variable to contain the value of 'from'
 Change reference to 'from' to use this new variable
 Assumptions: 'from' is not within the test of a loop or 'if'
              not currently traversing 'from' or the statement it is in

 */
 ROSE_DLL_API SgAssignInitializer* splitExpression(SgExpression* from, std::string newName = "");

//! Split long expressions into blocks of statements
ROSE_DLL_API void splitExpressionIntoBasicBlock(SgExpression* expr);

//! Remove labeled goto statements
ROSE_DLL_API void removeLabeledGotos(SgNode* top);

//! If the given statement contains any break statements in its body, add a new label below the statement and change the breaks into gotos to that new label.
ROSE_DLL_API void changeBreakStatementsToGotos(SgStatement* loopOrSwitch);

//! Check if the body of a 'for' statement is a SgBasicBlock, create one if not.
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsBodyOfFor(SgForStatement* fs);

//! Check if the body of a 'upc_forall' statement is a SgBasicBlock, create one if not.
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsBodyOfUpcForAll(SgUpcForAllStatement* fs);

//! Check if the body of a 'while' statement is a SgBasicBlock, create one if not.
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsBodyOfWhile(SgWhileStmt* ws);

//! Check if the body of a 'do .. while' statement is a SgBasicBlock, create one if not.
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsBodyOfDoWhile(SgDoWhileStmt* ws);

//! Check if the body of a 'switch' statement is a SgBasicBlock, create one if not.
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsBodyOfSwitch(SgSwitchStatement* ws);

//! Check if the body of a 'case option' statement is a SgBasicBlock, create one if not.
SgBasicBlock* ensureBasicBlockAsBodyOfCaseOption(SgCaseOptionStmt* cs);

//! Check if the body of a 'default option' statement is a SgBasicBlock, create one if not.
SgBasicBlock* ensureBasicBlockAsBodyOfDefaultOption(SgDefaultOptionStmt * cs);
    
//! Check if the true body of a 'if' statement is a SgBasicBlock, create one if not.
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsTrueBodyOfIf(SgIfStmt* ifs);

//! Check if the false body of a 'if' statement is a SgBasicBlock, create one if not when the flag is true.
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsFalseBodyOfIf(SgIfStmt* ifs, bool createEmptyBody = true);

//! Check if the body of a 'catch' statement is a SgBasicBlock, create one if not.
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsBodyOfCatch(SgCatchOptionStmt* cos);

//! Check if the body of a SgOmpBodyStatement is a SgBasicBlock, create one if not
ROSE_DLL_API SgBasicBlock* ensureBasicBlockAsBodyOfOmpBodyStmt(SgOmpBodyStatement* ompbodyStmt);

// DQ (1/18/2015): This is added to support better quality token-based unparsing.
//! Remove unused basic block IR nodes added as part of normalization.
ROSE_DLL_API void cleanupNontransformedBasicBlockNode();

// DQ (1/18/2015): This is added to support better quality token-based unparsing.
//! Record where normalization have been done so that we can preform denormalizations as required for the token-based unparsing to generate minimal diffs.
ROSE_DLL_API void recordNormalizations(SgStatement* s);

//! Check if a statement is a (true or false) body of a container-like parent, such as For, Upc_forall, Do-while,
//! switch, If, Catch, OmpBodyStmt, etc
bool isBodyStatement (SgStatement* s);

//! Fix up ifs, loops, while, switch, Catch, OmpBodyStatement, etc. to have blocks as body components. It also adds an empty else body to if statements that don't have them.
void changeAllBodiesToBlocks(SgNode* top, bool createEmptyBody = true);

// The same as changeAllBodiesToBlocks(SgNode* top). Phased out.
//void changeAllLoopBodiesToBlocks(SgNode* top);

//! Make a single statement body to be a basic block. Its parent is if, while, catch, or upc_forall etc.
SgBasicBlock * makeSingleStatementBodyToBlock(SgStatement* singleStmt);

#if 0
/**  If s is the body of a loop, catch, or if statement and is already a basic block,
 *   s is returned unmodified. Otherwise generate a SgBasicBlock between s and its parent
 *   (a loop, catch, or if statement, etc). */
SgLocatedNode* ensureBasicBlockAsParent(SgStatement* s);
#endif

//! Get the constant value from a constant integer expression; abort on
//! everything else.  Note that signed long longs are converted to unsigned.
unsigned long long getIntegerConstantValue(SgValueExp* expr);

//! Get a statement's dependent declarations which declares the types used in the statement. The returned vector of declaration statements are sorted according to their appearance order in the original AST. Any reference to a class or template class from a namespace will treated as a reference to the enclosing namespace.
std::vector<SgDeclarationStatement*> getDependentDeclarations (SgStatement* stmt );


//! Insert an expression (new_exp )before another expression (anchor_exp) has possible side effects, without changing the original semantics. This is achieved by using a comma operator: (new_exp, anchor_exp). The comma operator is returned.
SgCommaOpExp *insertBeforeUsingCommaOp (SgExpression* new_exp, SgExpression* anchor_exp);

//! Insert an expression (new_exp ) after another expression (anchor_exp) has possible side effects, without changing the original semantics. This is done by using two comma operators:  type T1; ... ((T1 = anchor_exp, new_exp),T1) )... , where T1 is a temp variable saving the possible side effect of anchor_exp. The top level comma op exp is returned. The reference to T1 in T1 = anchor_exp is saved in temp_ref.
SgCommaOpExp *insertAfterUsingCommaOp (SgExpression* new_exp, SgExpression* anchor_exp, SgStatement** temp_decl = NULL, SgVarRefExp** temp_ref = NULL);


/// \brief   moves the body of a function f to a new function f`;
///          f's body is replaced with code that forwards the call to f`.
/// \return  a pair indicating the statement containing the call of f`
///          and an initialized name refering to the temporary variable
///          holding the result of f`. In case f returns void
///          the initialized name is NULL.
/// \param   definingDeclaration the defining function declaration of f
/// \param   newName the name of function f`
/// \details f's new body becomes { f`(...); } and { int res = f`(...); return res; }
///          for functions returning void and a value, respectively.
///          two function declarations are inserted in f's enclosing scope
/// \code
///          result_type f`(...);                       <--- (1)
///          result_type f (...) { forward call to f` }
///          result_type f`(...) { original code }      <--- (2)
/// \endcode
///          Calls to f are not updated, thus in the transformed code all
///          calls will continue calling f (this is also true for
///          recursive function calls from within the body of f`).
///          After the function has created the wrapper,
///          definingDeclaration becomes the wrapper function
///          The definition of f` is the next entry in the
///          statement list; the forward declaration of f` is the previous
///          entry in the statement list.
/// \pre     definingDeclaration must be a defining declaration of a
///          free standing function.
///          typeid(SgFunctionDeclaration) == typeid(definingDeclaration)
///          i.e., this function is NOT implemented for class member functions,
///          template functions, procedures, etc.
std::pair<SgStatement*, SgInitializedName*>
wrapFunction(SgFunctionDeclaration& definingDeclaration, SgName newName);

/// \overload
/// \tparam  NameGen functor that generates a new name based on the old name.
///          interface: SgName nameGen(const SgName&)
/// \param   nameGen name generator
/// \brief   see wrapFunction for details
template <class NameGen>
std::pair<SgStatement*, SgInitializedName*>
wrapFunction(SgFunctionDeclaration& definingDeclaration, NameGen nameGen)
{
  return wrapFunction(definingDeclaration, nameGen(definingDeclaration.get_name()));
}

/// \brief convenience function that returns the first initialized name in a
///        list of variable declarations.
SgInitializedName& getFirstVariable(SgVariableDeclaration& vardecl);


//@}

// DQ (6/7/2012): Unclear where this function should go...
  bool hasTemplateSyntax( const SgName & name );

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
  ROSE_DLL_API bool isNonconstReference(SgType* t);
  ROSE_DLL_API bool isReferenceType(SgType* t);

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

// DQ (1/23/2013): Added support for generated a set of source sequence entries.
   std::set<unsigned int> collectSourceSequenceNumbers( SgNode* astNode );
      
//--------------------------------Type Traits (C++)---------------------------
      bool HasNoThrowAssign(const SgType * const inputType);
      bool HasNoThrowCopy(const SgType * const inputType);
      bool HasNoThrowConstructor(const SgType * const inputType);
      bool HasTrivialAssign(const SgType * const inputType);
      bool HasTrivialCopy(const SgType * const inputType);
      bool HasTrivialConstructor(const SgType * const inputType);
      bool HasTrivialDestructor(const SgType * const inputType);
      bool HasVirtualDestructor(const SgType * const inputType);
      bool IsBaseOf(const SgType * const inputBaseType, const SgType * const inputDerivedType);
      bool IsAbstract(const SgType * const inputType);
      bool IsClass(const SgType * const inputType);
      bool IsEmpty(const SgType * const inputType);
      bool IsEnum(const SgType * const inputType);
      bool IsPod(const SgType * const inputType);
      bool IsPolymorphic(const SgType * const inputType);
      bool IsStandardLayout(const SgType * const inputType);
      bool IsLiteralType(const SgType * const inputType);
      bool IsTrivial(const SgType * const inputType);
      bool IsUnion(const SgType * const inputType);
      SgType *  UnderlyingType(SgType *type);

// DQ (3/2/2014): Added a new interface function (used in the snippet insertion support).
//   void supportForInitializedNameLists ( SgScopeStatement* scope, SgInitializedNamePtrList & variableList );

// DQ (3/4/2014): Added support for testing two trees for equivalents using the AST iterators.
   bool isStructurallyEquivalentAST( SgNode* tree1, SgNode* tree2 );

// JP (10/14/24): Moved code to evaluate a const integer expression (like in array size definitions) to SageInterface
  /*! The datastructure is used as the return type for SageInterface::evaluateConstIntegerExpression(). One needs to always check whether hasValue_ is true before accessing value_ */
  struct const_int_expr_t {
    size_t value_;
    bool hasValue_;
  };
  /*! \brief The function tries to evaluate const integer expressions (such as are used in array dimension sizes). It follows variable symbols, and requires constness. */
  struct const_int_expr_t evaluateConstIntegerExpression(SgExpression *expr);

// JP (9/17/14): Added function to test whether two SgType* are equivalent or not
   bool checkTypesAreEqual(SgType *typeA, SgType *typeB);

//--------------------------------Java interface functions ---------------------
#ifdef ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
      ROSE_DLL_API std::string getTempDirectory(SgProject *project);
      ROSE_DLL_API void destroyTempDirectory(std::string);
      ROSE_DLL_API SgFile *processFile(SgProject *, std::string, bool unparse = false);
      ROSE_DLL_API std::string preprocessPackage(SgProject *, std::string);
      ROSE_DLL_API std::string preprocessImport(SgProject *, std::string);
      ROSE_DLL_API SgFile* preprocessCompilationUnit(SgProject *, std::string, std::string, bool unparse = true);
      ROSE_DLL_API SgClassDefinition *findJavaPackage(SgScopeStatement *, std::string);
      ROSE_DLL_API SgClassDefinition *findOrInsertJavaPackage(SgProject *, std::string, bool create_directory = false);
      ROSE_DLL_API SgClassDeclaration *findOrImportJavaClass(SgProject *, SgClassDefinition *package_definition, std::string);
      ROSE_DLL_API SgClassDeclaration *findOrImportJavaClass(SgProject *, std::string, std::string);
      ROSE_DLL_API SgClassDeclaration *findOrImportJavaClass(SgProject *, SgClassType *);
      ROSE_DLL_API SgMemberFunctionDeclaration *findJavaMain(SgClassDefinition *);
      ROSE_DLL_API SgMemberFunctionDeclaration *findJavaMain(SgClassType *);
#endif // ROSE_BUILD_JAVA_LANGUAGE_SUPPORT



// DQ (8/31/2016): Making this a template function so that we can have it work with user defined filters.
//! This function detects template instantiations that are relevant when filters are used.
/*!
    EDG normalizes some in-class template functions and member functions to be redefined outside of a class. this causes the associated template instantiations
    to be declared outside of the class, and to be marked as compiler generated (since the compiler generated form outside of the class declaration).
    ROSE captures the function definitions, but in the new location (defined outside of the class declaration).  This can confuse some simple tests
    for template instantiations that are a part of definitions in a file, thus we have this function to detect this specific normalization.
 */
template < class T >
bool isTemplateInstantiationFromTemplateDeclarationSatisfyingFilter (SgFunctionDeclaration* function, T* filter )
   {
  // DQ (9/1/2016): This function is called in the Call graph generation to avoid filtering out EDG normalized 
  // function template instnatiations (which come from normalized template functions and member functions).
  // Note that because of the EDG normailzation the membr function is moved outside of the class, and 
  // thus marked as compiler generated.  However the template instantiations are always marked as compiler 
  // generated (if not specializations) and so we want to include a template instantiation that is marked 
  // as compiler generated, but is from a template declaration that satisfyied a specific user defined filter.
  // The complexity of this detection is isolated here, but knowing that it must be called is more complex.
  // This function is call in the CG.C file of tests/nonsmoke/functional/roseTests/programAnalysisTests/testCallGraphAnalysis.

     bool retval = false;

#define DEBUG_TEMPLATE_NORMALIZATION_DETECTION 0

#if DEBUG_TEMPLATE_NORMALIZATION_DETECTION
     printf ("In isNormalizedTemplateInstantiation(): function = %p = %s = %s \n",function,function->class_name().c_str(),function->get_name().str());
#endif

  // Test for this to be a template instantation (in which case it was marked as 
  // compiler generated but we may want to allow it to be used in the call graph, 
  // if it's template was a part was defined in the current directory).
     SgTemplateInstantiationFunctionDecl*       templateInstantiationFunction       = isSgTemplateInstantiationFunctionDecl(function);
     SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunction = isSgTemplateInstantiationMemberFunctionDecl(function);

     if (templateInstantiationFunction != NULL)
        {
       // When the defining function has been normalized by EDG, only the non-defining declaration will have a source position.
          templateInstantiationFunction = isSgTemplateInstantiationFunctionDecl(templateInstantiationFunction->get_firstNondefiningDeclaration());
          SgTemplateFunctionDeclaration* templateFunctionDeclaration = templateInstantiationFunction->get_templateDeclaration();
          if (templateFunctionDeclaration != NULL)
             {
               retval = filter->operator()(templateFunctionDeclaration);
             }
            else
             {
             // Assume false.
             }

#if DEBUG_TEMPLATE_NORMALIZATION_DETECTION
          printf ("   --- case of templateInstantiationFunction: retval = %s \n",retval ? "true" : "false");
#endif
        }
       else
        {
          if (templateInstantiationMemberFunction != NULL)
             {
            // When the defining function has been normalized by EDG, only the non-defining declaration will have a source position.
               templateInstantiationMemberFunction = isSgTemplateInstantiationMemberFunctionDecl(templateInstantiationMemberFunction->get_firstNondefiningDeclaration());
               SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = templateInstantiationMemberFunction->get_templateDeclaration();
               if (templateMemberFunctionDeclaration != NULL)
                  {
                    retval = filter->operator()(templateMemberFunctionDeclaration);
                  }
                 else
                  {
                 // Assume false.
                  }

#if DEBUG_TEMPLATE_NORMALIZATION_DETECTION
               printf ("   --- case of templateInstantiationMemberFunction: retval = %s \n",retval ? "true" : "false");
#endif
             }
        }

     return retval;
   }

void detectCycleInType(SgType * type, const std::string & from);

}// end of namespace

#endif
