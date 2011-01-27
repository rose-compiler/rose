/**
 * \file RtedTransformation.h
 */
#ifndef RTEDTRANS_H
#define RTEDTRANS_H

#include <set>
#include <string>

#include "RtedSymbols.h"
#include "DataStructures.h"

#include "CppRuntimeSystem/rted_iface_structs.h"
#include "CppRuntimeSystem/rted_typedefs.h"

//
// convenience and debug functions
//

inline
SgScopeStatement* get_scope(SgInitializedName* initname)
{
  return initname ? initname->get_scope() : NULL;
}

/// \brief returns whether a name belongs to rted (i.e., has prefix "rted_")
bool isRtedDecl(const std::string& name);

/// \brief returns true if func points to the main function of a
///        C, C++, or UPC program.
/// \note  recognizes UPC main functions (as opposed to SageInterface::isMain)
// \pp \todo integrate into SageInterface::isMain
bool is_main_func(const SgFunctionDefinition* func);

/// \brief builds a UPC barrier statement
// \pp \todo integrate into SageBuilder
SgUpcBarrierStatement* buildUpcBarrierStatement();

/// \brief adds an AddressDesc for a single ptr/arr indirection
/// \param the base type (already after the indirection)
AddressDesc baseAddressDesc(const SgType* t);

//
// helper functions
//

/// \brief replaces double quote with single quote characters in a string
std::string removeSpecialChar(std::string str);

/// \brief  finds the first parent that is an SgStatement node
/// \return the parent statement or NULL if there is none
SgStatement* getSurroundingStatement(SgExpression* n);

/// \overload
SgStatement* getSurroundingStatement(SgInitializedName* n);

/// \brief   returns the base type for arrays and pointers
/// \param t a type
/// \return  the base_type, if it exists; t otherwise
/// \note    type-modifiers are currently not skipped (should they be?)
///          e.g., int* volatile X[] = /* ... */;
SgType* skip_ArrPtrType(SgType* t);

/// \brief  skips one modifier type node
/// \return the base type if t is an SgModifierType
///         t otherwise
SgType* skip_ModifierType(SgType* t);

/// \brief   returns true, iff name refers to a char-array modifying function (e.g., strcpy, etc.)
bool isStringModifyingFunctionCall(const std::string& name);

/// \brief Check if a function call is a call to a function on our ignore list.
/// \details We do not want to check those functions right now. This check makes
///          sure that we dont push variables on the stack for functions that
///          we dont check and hence the generated code is cleaner
bool isFunctionCallOnIgnoreList(const std::string& name);

/// \brief Check if a function call is a call to an IO function
bool isFileIOFunctionCall(const std::string& name);

/// \brief tests whether type is a C++ filestream (i.e., std::fstream)
bool isFileIOVariable(SgType* type);

/// \brief tests whether the declaration is a constructor
bool isConstructor( SgDeclarationStatement* decl );

/// \brief tests whether n was declared in a class / struct
bool isStructMember(const SgInitializedName& n);

/// \brief tests whether n is a function parameter
bool isFunctionParameter(const SgInitializedName& n);

//
// helper functions to insert rted checks

enum InsertLoc { ilAfter = 0, ilBefore = 1 };

SgExprStatement* insertCheck(InsertLoc iloc, SgStatement* stmt, SgFunctionSymbol* checker, SgExprListExp* args);

/// \brief   creates a statement node for calling the function checker with some arguments
///          and adds the check before the statement where checked_node is a part of
/// \return  the created statement node
SgExprStatement* insertCheckOnStmtLevel(InsertLoc iloc, SgExpression* checked_node, SgFunctionSymbol* checker, SgExprListExp* args);

/// \brief   creates a statement node for calling the function checker with some arguments
///          and adds depending on iloc adds the check before or after the statement where
///          checked_node is a part of
/// \return  the created statement node
SgExprStatement* insertCheck(InsertLoc iloc, SgStatement* stmt, SgFunctionSymbol* checker, SgExprListExp* args);

/// \brief   adds a comment in addition to creating a check
/// \return  the created statement node
SgExprStatement* insertCheck(InsertLoc iloc, SgStatement* stmt, SgFunctionSymbol* checker, SgExprListExp* args, const std::string& comment);

//
// functions that create AST nodes for the RTED transformations

/// \brief   creates an aggregate initializer expression with a given type
SgAggregateInitializer* genAggregateInitializer(SgExprListExp* initexpr, SgType* type);

/// \brief   creates a variable reference expression from a given name
SgVarRefExp* genVarRef( SgInitializedName* initName );

/* -----------------------------------------------------------
 * tps : 6March 2009: This class adds transformations
 * so that runtime errors are caught at runtime before they happen
 * -----------------------------------------------------------*/

class RtedTransformation : public AstSimpleProcessing {
private:
   enum ReadWriteMask { Read = 1, Write = 2, BoundsCheck = 4 };

   // \pp added an enum to give names to what were integer values before.
   //     Normal     ... affected size is the whole object
   //     Elem       ... affected size is an array element
   enum AppendKind { Whole = 0, Elem = 2 };

   // track the files that we're transforming, so we can ignore nodes and
   // references to nodes in other files
   std::set< std::string > *rtedfiles;

   // VARIABLES ------------------------------------------------------------
   // ------------------------ array ------------------------------------
   // The array of callArray calls that need to be inserted
   std::map<SgVarRefExp*, RtedArray*> create_array_define_varRef_multiArray;
   std::map<SgExpression*, RtedArray*> create_array_access_call;
   // remember variables that were used to create an array. These cant be reused for array usage calls
   std::vector<SgVarRefExp*> variablesUsedForArray;
   // this vector is used to check which variables have been marked as initialized (through assignment)

   typedef std::map<SgVarRefExp*,std::pair< SgInitializedName*, AllocKind> > InitializedVarMap;

   InitializedVarMap variableIsInitialized;
   // when traversing variables, we find some that are initialized names
   // instead of varrefexp, and so we create new varrefexps but we do
   // add them later and not during the same traversal.
   std::map<SgStatement*,SgStatement*> insertThisStatementLater;

   // the following stores all variables that are created (and used e.g. in functions)
   // We need to store the name, type and intialized value
public:
   // We need to store the variables that are being accessed
   std::map<SgInitializedName*, RtedArray*> create_array_define_varRef_multiArray_stack;
   std::vector<SgVarRefExp*> variable_access_varref;
   std::vector<SgInitializedName*> variable_declarations;
   std::vector<SgFunctionDefinition*> function_definitions;
   // function calls to free

   typedef std::vector< std::pair<SgExpression*, AllocKind> > Deallocations;
   Deallocations frees;
   // return statements that need to be changed
   std::vector< SgReturnStmt*> returnstmt;
   // Track pointer arithmetic, e.g. ++, --
   std::vector< SgExpression* > pointer_movements;
private:
   // map of expr ϵ { SgPointerDerefExp, SgArrowExp }, SgVarRefExp pairs
   // the deref expression must be an ancestor of the varref
   std::map<SgExpression*,SgVarRefExp*>  variable_access_pointerderef;
   // The second SgExpression can contain either SgVarRefExp,
   // or a SgThisExp
   std::map<SgExpression*, SgVarRefExp*> variable_access_arrowexp;
   std::map<SgExpression*, SgThisExp*>   variable_access_arrowthisexp;

   // ------------------------ string -----------------------------------
   // handle call to functioncall
   std::vector<RtedArguments*>         function_call;
   std::vector<SgFunctionCallExp*>     function_call_missing_def; ///< calls to functions whose
                                                                  ///  definitions we don't know, and thus, whose
                                                                  ///  signatures we must check at runtime
   std::vector<SgFunctionCallExp*>     reallocs;                  ///< function calls to realloc

public:
   std::vector<SgUpcBarrierStatement*> upcbarriers;


   // what statements we need to bracket with enter/exit scope calls
   typedef std::map<SgStatement*, SgNode*> ScopeMap;
   ScopeMap scopes;

   // store all classdefinitions found
   std::map<SgClassDefinition*,RtedClassDefinition*> class_definitions;

   // indicates if we have a globalconstructor build or not
   SgClassDeclaration* globConstructor;
   SgBasicBlock* globalFunction;
   SgVariableDeclaration* globalConstructorVariable;
   SgBasicBlock* buildGlobalConstructor(SgScopeStatement* scope, std::string name);
   SgBasicBlock* appendToGlobalConstructor(SgScopeStatement* scope, std::string name);
   void appendGlobalConstructor(SgScopeStatement* scope, SgStatement* stmt);
   void appendGlobalConstructorVariable(SgScopeStatement* scope,
         SgStatement* stmt);
   SgVariableDeclaration* getGlobalVariableForClass(SgGlobal* globel, SgClassDeclaration* classStmt);


   // The following are vars that are needed for transformations
   // and retrieved through the visit function
   SgClassSymbol* runtimeClassSymbol;
   SgScopeStatement* rememberTopNode;
   SgStatement* mainFirst;
   SgBasicBlock* mainBody;

   // FUNCTIONS ------------------------------------------------------------
   // Helper function

   // Transformation specific Helper Functions
   // Returns the defining definition for the function called by fn_call, if
   // possible.  If the direct link does not exist, will do a memory pool
   // traversal to find the definition.  May still return NULL if the definition
   // cannot be determined statically.
   SgFunctionDeclaration* getDefiningDeclaration( SgFunctionCallExp* fn_call );

   // insert: RuntimeSystem* runtimeSystem = new RuntimeSystem();
   void insertRuntimeSystemClass();
   void insertAssertFunctionSignature( SgFunctionCallExp* exp );
   void insertConfirmFunctionSignature( SgFunctionDefinition* fndef );
   void insertFreeCall(SgExpression* freeExp, AllocKind ak);
   void insertReallocateCall( SgFunctionCallExp* exp );

   /**
    * Appends all of the constructors of @c type to @c constructors.  The
    * constructors are those member functions whose name matches that of the
    * type.
    */
   void appendConstructors(SgClassDefinition* cdef, SgDeclarationStatementPtrList& constructors);

   /**
    * @return @c true @b iff @c exp is a descendent of an assignment expression
    * (such as @ref SgAssignmentOp or @ref SgPlusAssignOp)
    */
   bool isthereAnotherDerefOpBetweenCurrentAndAssign(SgExpression* exp );

public:
   /**
    * @return @c SgPointerType if @c type is a pointer type, reference to pointer
    * type or typedef whose base type is a pointer type, and @c null otherwise.
    */
   SgPointerType* isUsableAsSgPointerType( SgType* type );
   SgArrayType* isUsableAsSgArrayType( SgType* type );
   SgReferenceType* isUsableAsSgReferenceType( SgType* type );
   bool isInInstrumentedFile( SgNode* n );
   void visit_isArraySgAssignOp(SgAssignOp* const);

   void appendFileInfo( SgExprListExp* arg_list, SgStatement* stmt);
   void appendFileInfo( SgExprListExp* arg_list, SgScopeStatement* scope, Sg_File_Info* n);

   /// appends a function signature (typecount, returntype, arg1, ... argn)
   /// to the argument list.
   void appendSignature( SgExprListExp* arg_list, SgType* return_type, const SgTypePtrList& param_types);
private:

   bool isUsedAsLvalue( SgExpression* exp );
   /// is n a basic block, if statement, [do]while, or for statement
   bool isNormalScope( SgNode* n );
   SgExpression* getExprBelowAssignment(SgExpression* exp);

   // ********************* Deep copy classes in headers into source **********
   SgClassDeclaration* instrumentClassDeclarationIntoTopOfAllSourceFiles(SgProject* project, SgClassDeclaration* classDecl);
   bool hasPrivateDataMembers(SgClassDeclaration* cd_copy);
   void moveupPreprocessingInfo(SgProject* project);
   void insertNamespaceIntoSourceFile(SgSourceFile* sf);
   //typedef std::map<SgSourceFile*, std::pair < SgNamespaceDeclarationStatement*,
   //                                    SgNamespaceDeclarationStatement* > > SourceFileRoseNMType;
   typedef std::map<SgSourceFile*, SgNamespaceDeclarationStatement* > SourceFileRoseNMType;
   SourceFileRoseNMType sourceFileRoseNamespaceMap;
   //std::vector<std::string> classesInRTEDNamespace;
   std::map<SgClassDefinition*, SgClassDefinition*> classesInRTEDNamespace;
   bool hasClassConstructor(SgClassDeclaration* classdec);
   // ********************* Deep copy classes in headers into source **********


public:
    /// \brief   creates a "C-style constructor" from an aggregate initializer
    /// \details used, when aggregated values are passed as function arguments
    /// \code
    ///          foo( (CStyleCtorType) { 'a', "b", 3 } );
    /// \endcode
    SgCastExp* ctorTypeDesc(SgAggregateInitializer* exp) const;

    /// \brief   creates a "C-style constructor" for a rted_SourceInfo object
    ///          from an aggregate initializer
    SgCastExp* ctorSourceInfo(SgAggregateInitializer* exp) const;

    /// \brief   creates a "C-style constructor" for a rted_AddressDesc object
    ///          from an aggregate initializer
    SgCastExp* ctorAddressDesc(SgAggregateInitializer* exp) const;

    /// \brief   creates a variable length array (VLA) "constructor"
    ///          from a list of TypeDesc initializers.
    /// \details used, when the VLA is passed as function arguments
    /// \code
    ///          foo( (TypeDesc[]) { tdobj1, tdobj2 } );
    /// \endcode
    SgCastExp* ctorTypeDescList(SgAggregateInitializer* exp) const;

    /// \brief   creates an address descriptor
    SgAggregateInitializer* mkAddressDesc(AddressDesc desc) const;

    /// \brief   creates an address descriptor based on the type of an expression
    SgAggregateInitializer* mkAddressDesc(SgExpression* expr) const;

    /// \brief   creates a Sage representation of ak
    SgEnumVal* mkAllocKind(AllocKind ak) const;

    /// \brief     creates an expression constructing an rted_address
    /// \param exp an expression that will be converted into an address
    /// \param upcShared indicates whether the address is part of the PGAS
    SgFunctionCallExp* mkAddress(SgExpression* exp, bool upcShared) const;

    /// \brief returns the canonical pointer to the rted_TypeDesc type
    SgType* roseTypeDesc() const    { return symbols.roseTypeDesc; }

    /// \brief returns the canonical pointer to the rted_AddressDesc type
    SgType* roseAddressDesc() const { return symbols.roseAddressDesc; }

    /// \brief returns the canonical pointer to the rted_FileInfo type
    SgType* roseFileInfo() const    { return symbols.roseSourceInfo; }

public:
   /// \brief rewrites the last statement in main (see member variable mainLast)
   void insertMainCloseCall();

   void visit_isArraySgInitializedName(SgNode* n);
   void visit_isAssignInitializer(SgAssignInitializer* const n);

   void visit_isArrayPntrArrRefExp(SgPntrArrRefExp* const n);
   void visit_isArrayExprListExp(SgNode* n);
   void visit_isSgScopeStatement(SgScopeStatement* const n);

   void addPaddingToAllocatedMemory(SgStatement* stmt,  RtedArray* array);

   // Function that inserts call to array : runtimeSystem->callArray
   void insertArrayCreateCall(SgVarRefExp* n, RtedArray* value);
   void insertArrayCreateCall(SgInitializedName* initName,  RtedArray* value);
   void insertArrayCreateCall(SgStatement* stmt,SgInitializedName* initName,  SgVarRefExp* varRef, RtedArray* value);
   SgStatement* buildArrayCreateCall(SgInitializedName* initName, SgVarRefExp* varRef,
         RtedArray* array,SgStatement* stmt);

   void insertArrayAccessCall(SgExpression* arrayExp, RtedArray* value);
   void insertArrayAccessCall(SgStatement* stmt, SgExpression* arrayExp, RtedArray* array);

   std::pair<SgInitializedName*,SgVarRefExp*> getRightOfDot(SgDotExp* dot , std::string str, SgVarRefExp* varRef);
   std::pair<SgInitializedName*,SgVarRefExp*> getRightOfDotStar(SgDotStarOp* dot , std::string str, SgVarRefExp* varRef);
   std::pair<SgInitializedName*,SgVarRefExp*> getRightOfArrow(SgArrowExp* arrow , std::string str, SgVarRefExp* varRef);
   std::pair<SgInitializedName*,SgVarRefExp*> getRightOfArrowStar(SgArrowStarOp* arrowstar, std::string str, SgVarRefExp* varRef);
   std::pair<SgInitializedName*,SgVarRefExp*> getPlusPlusOp(SgPlusPlusOp* plus ,std::string str, SgVarRefExp* varRef);
   std::pair<SgInitializedName*,SgVarRefExp*> getMinusMinusOp(SgMinusMinusOp* minus ,std::string str, SgVarRefExp* varRef);
   std::pair<SgInitializedName*,SgVarRefExp*> getRightOfPointerDeref(SgPointerDerefExp* dot, std::string str, SgVarRefExp* varRef);

   SgVarRefExp* resolveToVarRefRight(SgExpression* expr);
   SgVarRefExp* resolveToVarRefLeft(SgExpression* expr);
   RtedSymbols  symbols;

   bool isVarRefInCreateArray(SgInitializedName* search);
   void insertFuncCall(RtedArguments* args);
   void insertIOFuncCall(RtedArguments* args);
   void visit_isFunctionCall(SgFunctionCallExp* const fcexp);

public:
   /// Visit pointer assignments whose lhs is computed from the original value of
   /// the pointer by virtue of the operator alone (e.g. ++, --)  As a heuristic,
   /// we say that such operations should not change the @e "Memory Chunk", i.e.
   /// the array the pointer refers to.
   void visit_pointer_movement( SgNode* node );
   /// Insert calls to registerPointerChange.  Don't worry about checkMemReads,
   /// those should be handled elsewhere (i.e. varref), but after the assignment,
   /// even if the memory was readable, ensure we stayed within array bounds.
   void insert_pointer_change( SgExpression* op );
private:
   // simple scope handling
   std::string scope_name( SgNode* n);
   void bracketWithScopeEnterExit( SgFunctionDefinition* fndef );
   void bracketWithScopeEnterExit( SgStatement* stmt_or_block, Sg_File_Info* exit_file_info );


   // is it a variable?
   void visit_isSgVariableDeclaration(SgNode* n);
   void insertCreateObjectCall( RtedClassDefinition* cdef );
   void insertVariableCreateCall(SgInitializedName* initName);
   void insertVariableCreateCall(SgInitializedName* initName,SgExpression* expr);
   bool isVarInCreatedVariables(SgInitializedName* n);
   void insertInitializeVariable(SgInitializedName*, SgExpression*, AllocKind);
   SgExpression* buildVariableInitCallExpr(SgInitializedName*, SgExpression*, SgStatement*, AllocKind);
   SgFunctionCallExp* buildVariableCreateCallExpr(SgInitializedName* name, SgStatement* stmt, bool forceinit=false);
   // TODO 2 djh: test docs
   /**
    * @b{ For Internal Use Only }.  See the overloaded convenience functions.
    */
   SgFunctionCallExp* buildVariableCreateCallExpr( SgExpression* var_ref, std::string& debug_name, bool init );

   SgExprStatement* buildVariableCreateCallStmt( SgInitializedName* name, SgStatement* stmt, bool forceinit=false );
   /**
    * @b{ For Internal Use Only }.  See the overloaded convenience functions.
    */
   SgExprStatement* buildVariableCreateCallStmt( SgFunctionCallExp* create_var_fn_call );

   void insertVariableCreateInitForParams( SgFunctionDefinition* n);
   void insertAccessVariable(SgVarRefExp* varRefE,SgExpression* derefExp);
   void insertAccessVariable(SgThisExp* varRefE,SgExpression* derefExp);
   void insertAccessVariable(SgScopeStatement* scope,
         SgExpression* derefExp, SgStatement* stmt, SgExpression* varRefE);
   void addFileIOFunctionCall(SgVarRefExp* n, bool read);
   void insertCheckIfThisNull(SgThisExp* texp);

public:
   void visit_isSgVarRefExp(SgVarRefExp* n, bool isRightBranchOfBinaryOp, bool thinkItsStopSearch);
   void visit_isSgArrowExp(SgArrowExp* const n);
   void visit_isSgPointerDerefExp(SgPointerDerefExp* const);
private:


   bool traverseAllChildrenAndFind(SgExpression* varRef, SgStatement* stmt);
   bool traverseAllChildrenAndFind(SgInitializedName* varRef, SgStatement* stmt);


   /// Renames the original main function
   /// copied from projects/UpcTranslation/upc_translation.C
   void renameMain(SgFunctionDeclaration * sg_func);
   void changeReturnStmt(SgReturnStmt * rstmt);

   /// factors commonalities of heap allocations
   void arrayHeapAlloc(SgInitializedName*, SgVarRefExp*, SgExpression*, AllocKind);

   /// creates a heap array record for a single argument allocation (e.g., malloc)
   void arrayHeapAlloc1(SgInitializedName*, SgVarRefExp*, SgExpressionPtrList&, AllocKind);

   /// creates a heap array record for a two argument allocation (e.g., calloc)
   void arrayHeapAlloc2(SgInitializedName*, SgVarRefExp*, SgExpressionPtrList&, AllocKind);

   AllocKind arrayAllocCall(SgInitializedName*, SgVarRefExp*, SgExprListExp*, SgFunctionDeclaration*);
   AllocKind arrayAllocCall(SgInitializedName*, SgVarRefExp*, SgExprListExp*, SgFunctionRefExp*, AllocKind);

public:
   RtedTransformation()
   : AstSimpleProcessing(),

     rtedfiles(NULL),

     create_array_define_varRef_multiArray(),
     create_array_access_call(),
     variablesUsedForArray(),
     pointer_movements(),
     variable_access_pointerderef(),
     variable_access_arrowexp(),
     variable_access_arrowthisexp(),
     function_call(),
     function_call_missing_def(),
     reallocs(),
     upcbarriers(),
     scopes(),
     globConstructor(false),
     globalFunction(NULL),
     globalConstructorVariable(NULL),
     mainFirst(NULL),
     mainBody(NULL),
     symbols()
   {}

   virtual ~RtedTransformation() {}

   // PUBLIC FUNCTIONS ------------------------------------------------------------
   // Insert Header Files
   void insertProlog(SgProject* proj);

   // analyse file and apply necessary (call) transformations
   void transform(SgProject* project, std::set<std::string> &rtedfiles);

   // Run frontend and return project
   SgProject* parse(int argc, char** argv);
   void loadFunctionSymbols(SgProject* project);

   SgAggregateInitializer* mkTypeInformation(SgInitializedName* initName);
   SgAggregateInitializer* mkTypeInformation(SgInitializedName* initName, SgType* type);
   SgAggregateInitializer* mkTypeInformation(SgType* type, bool resolve_class_names = true, bool array_to_pointer=false);

   /// \brief appends the array dimensions to the argument list
   void appendDimensions(SgExprListExp* arg_list, RtedArray*);

   /// \brief appends the array dimensions to the argument list if needed
   ///        (i.e., rce is a RtedClassArrayElement)
   void appendDimensionsIfNeeded(SgExprListExp* arg_list, RtedClassElement* rce);

   void appendAddressAndSize(SgExprListExp* arg_list, AppendKind ak, SgScopeStatement* scope, SgExpression* varRef, SgClassDefinition* cd);
   void appendAddressAndSize(SgExprListExp* arg_list, AppendKind ak, SgExpression* exp, SgType* type, SgClassDefinition* isUnionClass);

   void appendAddress( SgExprListExp* arg_list, SgExpression* exp );
   void appendClassName( SgExprListExp* arg_list, SgType* type );

   /**
    * Handle instrumenting function calls in for initializer statements, which may
    * contain variable declarations.  The basic approach is to instead add the
    * function calls to the test, and ensuring that:
    *
    * 		-	The original test's truth value is used as the truth value of the
    * 			new expression.
    * 		-	The instrumented function calls are invoked only once.
    *
    * 	Note that this will only work for function calls that return a value
    * 	suitable for bitwise operations.
    *
    * 	@param	exp				An expression, which must be a legal operand to a
    * 										bitwise operator.  It will be added to the for loop's
    * 										test in a way to make it as semantically equivalent as
    * 										possible as adding it to the initializer statement.
    *
    * 	@param	for_stmt	The for statement to add @c exp to.
    */
   void prependPseudoForInitializerExpression( SgExpression* exp, SgStatement* for_stmt );

   bool isGlobalExternVariable(SgStatement* stmt);

   void insertRegisterTypeCall(RtedClassDefinition* const rtedClass);
   void visit_isClassDefinition(SgClassDefinition* const cdef);

   void executeTransformations();
   void insertNamespaceIntoSourceFile(  SgProject* project, std::vector<SgClassDeclaration*> &traverseClasses);

   void populateDimensions( RtedArray* array, SgInitializedName* init, SgArrayType* type );
   void visit_checkIsMain(SgFunctionDefinition* const);

   //
   // implemented in RtedTransf_Upc.cpp

   /// \brief transforms a UPC barrier statement
   void transformUpcBarriers(SgUpcBarrierStatement* stmt);


   //
   // dependencies on AstSimpleProcessing
   //   (see also comment in RtedTransformation.cpp)

   virtual void visit(SgNode* n); // needed for the class extraction
};


//
// Access Functions added to treat UPC-forall and C/C++ for loops
//   somewhat uniformly
//

namespace GeneralizdFor
{
  /// \brief tests whether a node is either a C/C++ for loop, or a UPC forall loop.
  /// \return a pointer to a SgStatement if the argument points to a for-loop.
  ///         NULL, otherwise.
  SgStatement* is(SgNode* astNode);

  /// \brief returns the loop test of a generilized for statement
  SgStatement* test(SgStatement* astNode);

  /// \brief returns the initializer statement of a generilized for statement
  SgForInitStatement* initializer(SgStatement* astNode);
}

#endif
