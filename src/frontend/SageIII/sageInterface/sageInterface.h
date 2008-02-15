#ifndef ROSE_SAGE_INTERFACE
#define ROSE_SAGE_INTERFACE

// Liao 1/7/2008: start to move AST utility functions into this namespace, 
// see the rear of the file for new SAGE interfaces
//
// Major AST manipulation functions are scattered in the following directories 
// * src/midend/astUtil/astInterface
// * src/roseSupport
// * src/util
// * src/frontend/SageIII/sageInterface
// * project/OpenMP_Translator/ AST_Tools class
//
// Last modified: 1/11/2008
//------------------------------------------------------------------------------
// DQ (8/19/2004): Moved from ROSE/src/midend/astRewriteMechanism/rewrite.h
// Added global function for getting the string associated 
// with an enum (which is defined in global scope)
std::string getVariantName (VariantT v);

// DQ (12/9/2004): Qing, Rich and Dan have decided to start this namespace within ROSE
// This namespace is specific to interface funcions that operate on the Sage III AST.
// The name was choosen so as not to conflict with other classes within ROSE.
// This will become the future home of many interface functions which operate on
// the AST and which are generally useful to users.  As a namespace multiple files can be used
// to represent the compete interface and different developers may contribute interface
// functions easily.

// Constructor handling:
//     We could add simpler layers of support for construction of IR nodes by 
// hidding many details in "makeSg***()" functions.  such functions would
// return pointers to the associated Sg*** objects and would be able to hide
// many IR specific details, including:
//      memory handling
//      optional parameter settings not often required
//      use of Sg_File_Info objects (and setting them as transformations)
//      

// namespace AST_Interface  (this name is taken already by some of Qing's work :-)

/*! \brief This namespace is to organize functions that are useful when operating on the AST.

  \defgroup frontendSageUtilityFunctions SAGE III utility functions
  \ingroup ROSE_FrontEndGroup

    The Sage III IR design attempts to be minimalist.  thus additional functionality is
intended to be presented using separate higher level interfaces which work with the IR.
this namespace collects functions that operate on the IR and are suppotive of numerous types of
routine operations required to support general analysis and transformation of the AST.

    \internal Further organization of the functions in this namespace is required.

    \todo A number of additional things to do:
         - Pull scope handling out of EDG/Sage III translation so that is is made 
           available to anyone else building the Sage III IR from scratch (whch 
           when it gets non-trivial, envolves the manipulation of scopes).
         - Other stuff ...
 */
namespace SageInterface
{
//! an alias for Sg_File_Info::generateDefaultFileInfoForTransformationNode()
#define TRANS_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()
//! an internal counter for generating unqiue SgName
extern int gensym_counter;

//-------------------------------symbol table handling -------------------
//------------------------------------------------------------------------
  // DQ (11/24/2007): Functions moved from the Fortran support so that they could be called from within astPostProcessing.
  SgFunctionSymbol *lookupFunctionSymbolInParentScopes (const SgName &	functionName,
							SgScopeStatement *currentScope);

  // Liao 1/22/2008, used for get symobls for generating variable reference nodes
  //! find a symbol in current and ancestor scopes for a given variable name
  SgSymbol *lookupSymbolInParentScopes (const SgName &	name,
							SgScopeStatement *currentScope);
  /*! \brief set_name of symbol in symbol table.

      This function extracts the symbol from the relavant symbol table, 
      changes the name (at the declaration) and reinserts it into the 
      symbol table.

      \internal  I think this is what this function does, I need to double check.
   */
  // DQ (12/9/2004): Moved this function (by Alin Jula) from being a member of SgInitializedName
  // to this location where it can be a part of the interface for the Sage III AST.
  int set_name (SgInitializedName * initializedNameNode, SgName new_name);

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

  /*! \brief Generate a useful name to describe the SgNode

      \internal default names are used for SgNode objects that can not be associated with a name.
   */
  // DQ (9/21/2005): General function for extracting the name of declarations (when they have names)
    std::string get_name (const SgNode * node);
  //---------------------output symbol tables---------------

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

  //SgNode::get_globalFunctionTypeTable() ;
//---------------------------------- class utilities----------------------
//------------------------------------------------------------------------
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

  /*! \brief Return true if template definition is in the class, false if outside of class.
   */
  // DQ (8/27/2005): 
  bool templateDefinitionIsInClass (SgTemplateInstantiationMemberFunctionDecl
				    * memberFunctionDeclaration);

//--------------------------- function tools------------------------------
//------------------------------------------------------------------------

  /*! \brief Return true if function is overloaded.
   */
  // DQ (8/27/2005):
  bool isOverloaded (SgFunctionDeclaration * functionDeclaration);

  /*! \brief Generate a non-defining (forward) declaration from a defining function declaration.
   */
  // DQ (9/17/2005):
     SgTemplateInstantiationMemberFunctionDecl*
     buildForwardFunctionDeclaration
        (SgTemplateInstantiationMemberFunctionDecl * memberFunctionInstantiation);

//--------------------------- AST properties -----------------------------
//------------------------------------------------------------------------
//  std::string version();  // utility_functions.h, version number
  /*! brief These traverse the memory pool of SgFile IR nodes and determine what laguages are in use!
   */
  bool is_C_language ();
  bool is_C99_language ();
  bool is_Cxx_language ();
  bool is_Fortran_language ();
  bool is_binary_executable();
  bool is_mixed_C_and_Cxx_language ();
  bool is_mixed_Fortran_and_C_language ();
  bool is_mixed_Fortran_and_Cxx_language ();
  bool is_mixed_Fortran_and_C_and_Cxx_language ();


//------------------------entity generators  -----------------------------
//------------------------------------------------------------------------

  // DQ (6/22/2005):
  /*! \brief Generate unique name from C and C++ constructs.

      This is support for the AST merge, but is generally useful as a more general mechanism than 
      name mangling which is more closely ties to the generation of names to support link-time function name 
      resolution.  This is more general than common name mangling in that it resolves more relavant differences
      between C and C++ declarations. (e.g. the type within the declaration: "struct { int:8; } foo;").

      \implementation current work does not support expressions.

   */
    std::string generateUniqueName (SgNode * node,
				    bool
				    ignoreDifferenceBetweenDefiningAndNondefiningDeclarations);

  // DQ (3/10/2007): Generate a unique string from the source file position information
    std::string declarationPositionString (SgDeclarationStatement *
					   declaration);

  // DQ (1/20/2007): Added mechanism to generate project name from list of file names
    std::string generateProjectName (const SgProject * project);

  /*! \brief Returns STL vector of SgFile IR node pointers. 

      Demonstrates use of restrivcted traversal over just SgFile IR nodes.
   */
    std::vector < SgFile * >generateFileList ();

  // DQ (10/6/2006): Added support for faster mangled name generation (caching avoids recomputation).
  /*! \brief Support for faster mangled name generation (caching avoids recomputation).

   */
  void clearMangledNameCache (SgGlobal * globalScope);
  void resetMangledNameCache (SgGlobal * globalScope);
    std::string getMangledNameFromCache (SgNode * astNode);
    std::string addMangledNameToCache (SgNode * astNode,
				       const std::string & mangledName);

//------------------------------- scope ----------------------------------
//------------------------------------------------------------------------
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

  SgNamespaceDefinitionStatement * enclosingNamespaceScope (SgDeclarationStatement * declaration);
//  SgNamespaceDefinitionStatement * getEnclosingNamespaceScope (SgNode * node);

  bool isPrototypeInScope (SgScopeStatement * scope,
			   SgFunctionDeclaration * functionDeclaration,
			   SgDeclarationStatement * startingAtDeclaration);
  // Liao, 1/9/2008
  /*! 
  	\brief return the first global scope under current project
  */
  SgGlobal * getFirstGlobalScope(SgProject *project);

  /*!
	\brief get the last statement within a scope, return NULL if does not exit
  */
  SgStatement* getLastStatement(SgScopeStatement *scope);

  //SgFunctionDeclaration* findMain(SgNode* n) 
  // int get_scope_level(SgNode* astNode);

//-------------------source position: Sg_File_Info------------------------
//------------------------------------------------------------------------
  void setSourcePosition (SgLocatedNode * locatedNode);

  // Liao, 1/8/2007, set file info. for a whole subtree as transformation generated
//! set current node's source position as transformation generated
  void setOneSourcePositionForTransformation(SgNode *node);

//! recursively set source position info(Sg_File_Info) as transformation generated
  void setSourcePositionForTransformation (SgNode * root);

//! set source position info(Sg_File_Info) as transformation generated for all SgNodes 
  void setSourcePositionForTransformation_memoryPool();


//----------------------------misc. utilities ----------------------------
//------------------------------------------------------------------------
  SgDeclarationStatement * getNonInstantiatonDeclarationForClass
    (SgTemplateInstantiationMemberFunctionDecl * memberFunctionInstantiation);


  // DQ (10/14/2006): This function tests the AST to see if for a non-defining declaration, the 
//! check if defining declaration comes before of after the non-defining declaration.
  // bool declarationPreceedsDefinition ( SgClassDeclaration* classNonDefiningDeclaration, SgClassDeclaration* classDefiningDeclaration );
  bool declarationPreceedsDefinition (SgDeclarationStatement *
				      nonDefiningDeclaration,
				      SgDeclarationStatement *
				      definingDeclaration);

  // DQ (10/19/2006): Function calls have interesting context dependent rules to determine if 
  // they are output with a global qualifier or not.  Were this is true we have to avoid global 
  // qualifiers, since the function's scope has not been defined.  This is an example of where
  // qualification of function names in function calls are context dependent; an interesting
  // example of where the C++ language is not friendly to source-to-source processing :-).
  bool
    functionCallExpressionPreceedsDeclarationWhichAssociatesScope
    (SgFunctionCallExp * functionCall);

 /*! \brief Compute the intersection set for two ASTs.

     This is part of a test done by the copy function to compute those IR nodes in the copy that still reference the original AST.
  */
    std::vector < SgNode * >astIntersection (SgNode * original, SgNode * copy,
					     SgCopyHelp * help = NULL);

// deepCopy of AST subtree, Liao, Jan. 15, 2008
//! used for deep copy
   static SgTreeCopy g_treeCopy; 
//! deep copy a subtree
   SgNode* deepCopy (const SgNode* subtree);

//--------------------------------access functions---------------------------
//----------------------------------get/set sth.-----------------------------

// from VarSym.cc in src/midend/astOutlining/src/ASTtools
//! Get the variable symbol for the first initialized name of a declaration stmt.
  SgVariableSymbol* getFirstVarSym (SgVariableDeclaration* decl);

//---------------------------AST Data types-------------------------------
//------------------------------------------------------------------------
// from src/midend/astInlining/typeTraits.h
// src/midend/astUtil/astInterface/AstInterface.h

//! get the right bool type according to C or C++ language input
SgType* getBoolType(SgNode* n);

//! Is a type default constructible?  This may not quite work properly.
bool isDefaultConstructible(SgType* type);

//! Is a type copy constructible?  This may not quite work properly.
bool isCopyConstructible(SgType* type);

//! Does a type have a trivial (built-in) destructor?
bool hasTrivialDestructor(SgType* t);

//! Is this type a non-constant reference type?
bool isNonconstReference(SgType* t);

//! Is this type a const or non-const reference type?
bool isReferenceType(SgType* t);

//! Is this a const type?
bool isConstType(SgType* t);

//! Is this a scalar type?
bool isScalarType(SgType* t);

//--------------------------------loop handling --------------------------
//------------------------------------------------------------------------
// by Jeremiah
//! Add a step statement to the end of a loop body
//! Add a new label to the end of the loop, with the step statement after
//! it; then change all continue statements in the old loop body into
//! jumps to the label
//!
//! For example:
//! while (a < 5) {if (a < -3) continue;} (adding "a++" to end) becomes
//! while (a < 5) {if (a < -3) goto label; label: a++;}
template <class LoopStatement>
void addStepToLoopBody(LoopStatement* loopStmt, SgStatement* step);

void moveForStatementIncrementIntoBody(SgForStatement* f);
void convertForToWhile(SgForStatement* f);
void convertAllForsToWhiles(SgNode* top);
//! change continue statements in a given block of code to gotos to a label
void changeContinuesToGotos(SgStatement* stmt, SgLabelSymbol* label);

//-----------------------topdown search and find -------------------------
//------------------------------------------------------------------------
  /*! \brief top-down traversal from current node to find the main() function declaration
  */
   SgFunctionDeclaration* findMain(SgNode* currentNode);  

	  //midend/programTransformation/partialRedundancyElimination/pre.h
  //! find referenced symbols within an expression
   std::vector<SgVariableSymbol*> getSymbolsUsedInExpression(SgExpression* expr);

  //loopHelpers.h: already included into rose.h and its functons are put in global scope
  // should be include them here or not? 
  
//! Find break statements inside a particular statement, stopping at nested loops or switchs
//! loops or switch statements defines their own contexts for break
//! statements.  The function will stop immediately if run on a loop or switch
//! statement.
  std::vector<SgBreakStmt*> findBreakStmts(SgStatement* code); 

//! Find all continue statements inside a particular statement, stopping at nested loops
//! Nested loops define their own contexts for continue statements.  The
//! function will stop immediately if run on a loop.
  std::vector<SgContinueStmt*> findContinueStmts(SgStatement* code);
  std::vector<SgGotoStatement*> findGotoStmts(SgStatement* scope, SgLabelStatement* l);
  std::vector<SgReturnStmt*> findReturnStmts(SgStatement* scope);
  std::vector<SgStatement*> getSwitchCases(SgSwitchStatement* sw); 

//---------------------------bottom up search-----------------------------
//----------------------find enclosing(ancestor) SgXXX -------------------
/*! \brief traverses through the parents to the first scope of a given node
*/
  SgScopeStatement* getScope(const SgNode* astNode); 
  SgFunctionDefinition* getEnclosingProcedure(SgNode* n);
  SgSwitchStatement* findEnclosingSwitch(SgStatement* s);

  SgFunctionDeclaration * getEnclosingFunctionDeclaration (SgNode * astNode);
  SgFunctionDefinition* getEnclosingFunctionDefinition(SgNode* astNode);

   //roseSupport/utility_functions.h
  SgFile* getEnclosingFileNode ( const SgNode* astNode );
//! Get the initializer containing an expression if it is within an initializer.
  SgInitializer* getInitializerOfExpression(SgExpression* n);

#if 0
//---------------------------bottom up search-----------------------------
//----------------------find enclosing(ancestor) SgXXX -------------------
// get ancestor nodes, using bottom up/reverse search
// a universal reverse traveral interface 
// remember to put const to all arguments.

// what interface name is better?   getEnclosingXXX() vs. getAncestorXXX()

   // generic functions for base implementation
   // reverse search for nodes of type VariantT, until reaching endNode

   SgNode * getEnclosingSgNode(SgNode* source,VariantT, SgNode* endNode=NULL);
   std::vector<SgNode *>
            getAllEnclosingSgNode(SgNode* source,VariantT, SgNode* endNode=NULL);

   SgFile* getFile( const SgNode* astNode);
   SgGlobal* getGlobalScope( const SgNode* astNode);


   SgClassDeclaration* getEnclosingClassDeclaration( const SgNode* astNode);
   SgClassDefinition* getEnclosingClassDefinition( const SgNode* astNode);

  SgVariableDeclaration* findVariableDeclaratin( const string& varname)

   // e.g. for some expression, find its parent statement
   SgStatement* getEnclosingStatement(const SgNode* astNode);

   SgSwitchStatement* getEnclosingSwitch(SgStatement* s); 
   SgModuleStatement* getEnclosingModuleStatement( const SgNode* astNode);
  
  // used to build a variable reference for compiler generated code in current scope
   SgSymbol * findReachingDefinition (SgScopeStatement* startScope, SgName &name);

//------------------------AST traversal/walk-----------------------------
//------------------------------------------------------------------------

  // preorder traversal from current SgNode till find next SgNode of type V_SgXXX
  // default is any node
  //SgNode* getNextSgNode( const SgNode* currentNode, VariantT=0);// VariantT 0 is valid!

  SgNode* getNextSgNode( const SgNode* currentNode, VariantT=V_SgNode);

// Question: cross scope boundary or not? 
// bool crossScope==true, 

  SgStatement* getNextStatement(SgStatement * currentStmt, bool crossScope=true);
  SgStatement* getPreviousStatement(SgStatement * currentStmt,bool crossScope=true);

  SgStatement* getFirstStatement(SgScopeStatement *scope);
  SgStatement* getLastStatement(SgScopeStatement *scope);

//------------------------AST dump, stringify-----------------------------
//------------------------------------------------------------------------
  void generatePDF ( const SgProject & project ); //utility_functions.h
  void generateDOT ( const SgProject & project );

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

//-------------------------------Symbol table ----------------------------
//------------------------------------------------------------------------
//   current support from SgScopeStatement is quite good
// 

  SgFunctionSymbol *lookupFunctionSymbolInParentScopes (const SgName &  functionName,
                                                        SgScopeStatement *currentScope);
  void rebuildSymbolTable (SgScopeStatement * scope);
  void outputLocalSymbolTables (SgNode * node);
  void outputGlobalFunctionTypeSymbolTable ();

//--------------------------AST modification------------------------------
//------------------------------------------------------------------------
// any operations changing AST tree, including
// insert, copy, delete(remove), replace

  // insert before or after some point, argument list is consistent with LowLevelRewrite
  void insertAst(SgNode* targetPosition, SgNode* newNode, bool insertBefore=true);
  void insertStatement(SgStatement* targetStmt, SgStatement* newStmt, bool insertBefore=true);

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

  void replaceAst(SgNode* oldNode, SgNode* newNode);
  void replaceStatement (SgStatement* s_cur, SgStatement* s_new);

  // replace variable reference expression, used for variable substitution
  void replaceVarRefExp (SgExpression *oldExp, SgExpression *newExp);

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

  //src/midend/astInlining/inlinerSupport.C
  void removeUnusedLabels(SgNode* top)


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

  bool isMain (const SgFunctionDeclaration* decl);
  bool isOverloaded (SgFunctionDeclaration * functionDeclaration);

  bool isSwitchCond (const SgStatement* s);
  bool isIfCond (const SgStatement* s);
  bool isWhileCond (const SgStatement* s);
  bool isStdNamespace (const SgScopeStatement* scope);
  bool isTemplateInst (const SgDeclarationStatement* decl);


  bool isCtor (const SgFunctionDeclaration* func);
  bool isDtor (const SgFunctionDeclaration* func);

   // src/midend/astInlining/typeTraits.h
  bool isDefaultConstructible(SgType* type);
  bool isCopyConstructible(SgType* type);
  bool hasTrivialDestructor(SgType* t);
  bool isNonconstReference(SgType* t);
  bool isReferenceType(SgType* t);

  //  generic ones, or move to the SgXXX class as a member function

  bool isConst(SgNode* node); // const type, variable, function, etc.
  bool isStatic(SgNode* node); // const type, variable, function, etc.
  // .... and more

  bool isConstType (const SgType* type);
  bool isConstFunction (const SgFunctionDeclaration* decl);

  bool isStaticFunction (const SgFunctionDeclaration* func);
  bool isExternC (const SgFunctionDeclaration* func);

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

//***********************************************************************
//     operations for special types of AST nodes
//     should move them into SgXXX as member funtions?
//***********************************************************************

//------------------------ loop handling ---------------------------------
//------------------------------------------------------------------------
  //get and set loop control expressions 
  // 0: init expr, 1: condition expr, 2: stride expr

  SgExpression* getForLoopTripleValues(int valuetype,SgForStatement* forstmt );
  int setForLoopTripleValues(int valuetype,SgForStatement* forstmt, SgExpression* exp);

  bool isLoopIndexVarRef(SgForStatement* forstmt, SgVarRefExp *varref);
  SgInitializedName * getLoopIndexVar(SgForStatement* forstmt);

  inline SgExpression* getLoopConditionRoot(Loop* loop)
  void setLoopConditionRoot(Loop* loop, SgExpression* body)

  	//src/midend/astInlining/replaceExpressionWithStatement.h
        // handle several variants of loops: for-loop, while-loop,etc.
  template <class Loop>
  inline SgBasicBlock* getLoopBody(Loop* loop)
  void setLoopBody(Loop* loop, SgBasicBlock* body)

//------------------------expressions-------------------------------------
//------------------------------------------------------------------------
  //src/midend/programTransformation/partialRedundancyElimination/pre.h
  int countComputationsOfExpressionIn(SgExpression* expr, SgNode* root);

  	//src/midend/astInlining/replaceExpressionWithStatement.h
  void replaceAssignmentStmtWithStatement(SgExprStatement* from,                                        StatementGenerator* to);

  void replaceSubexpressionWithStatement(SgExpression* from,
                                       StatementGenerator* to);

  void replaceExpressionWithExpression(SgExpression* from, SgExpression* to);

  SgAssignInitializer* splitExpression(SgExpression* from,
                                     std::string newName = "");
	  //src/midend/astInlining/replaceExpressionWithStatement.h:
  SgExpression* getRootOfExpression(SgExpression* n);

//--------------------------preprocessing info. -------------------------
//------------------------------------------------------------------------

  void attachComment (const std::string& comment, SgStatement* s);
  void attachComment (const char* comment, SgStatement* s);

  //! Inserts an '#include' of a header file at the first statement.
  bool insertHeader (const std::string& filename, SgProject* proj);

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

  //! Moves preprocessingInfo of stmt_old to stmt_new.
  //  useful before inserting stmt_new before stmt_old
  void moveUpPreprocInfo (SgStatement* stmt_new, SgStatement* stmt_old);

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

  //! Dumps a statement's preprocessing information.
  void dumpPreprocInfo (const SgStatement* s, std::ostream& o);

//--------------------------------opeartor--------------------------------
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

//---------------------------AST Data types-------------------------------
//------------------------------------------------------------------------
// additional suggestion: macros for basic type creation, less typing and hide details
  // but has to be in global scope
  #define TypeInt SgTypeInt::createType() 
  #define TypeFloat SgTypeFloat::createType() 

 // no need for this? 
  std::string getTypeName ( SgType* type );//SgType::get_mangled()
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
  bool  isExtern()
  void  clearExtern()

   // similarly for other declararations and other properties
  void setExtern (SgVariableDeclaration*)
  void setPublic()
  void setPrivate()
//-----------------------topdown search and find -------------------------
//------------------------------------------------------------------------
// get descendant nodes, using topdown/preorder  search
// name convention: get or find ?
  // we can have two generic (template) functions
  // 1. preorder traversal from current SgNode till find next SgNode of type V_SgXXX
  //    until reach the end node
  SgNode* getNextSgNode( const SgNode* astSourceNode, VariantT=V_SgNode, SgNode* astEndNode=NULL);

  // 2. return all nodes of type VariantT following the source node
  std::vector<SgNode*> getAllNextSgNode( const SgNode* astSourceNode, VariantT=V_SgNode, SgNode* astEndNode=NULL);



#endif 
}// end of namespace

#endif
