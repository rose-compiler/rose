/**
 * \file RtedTransformation.h
 */
#ifndef RTEDTRANS_H
#define RTEDTRANS_H

#include <set>
#include <vector>
#include <map>
#include <string>

#include "RtedSymbols.h"
#include "DataStructures.h"

#include "CppRuntimeSystem/rted_iface_structs.h"
#include "CppRuntimeSystem/rted_typedefs.h"

enum { RTEDDEBUG = 1 };

/// analyzed files
typedef std::set<std::string> RtedFiles;

/// options that can be set on the command line
struct RtedOptions
{
  bool globalsInitialized;
};


/// \brief returns the language of the specified source file
SourceFileType fileType(const std::string& filename);

/// \overload
SourceFileType fileType(const SgSourceFile& sf);

//
// convenience and debug functions
//

inline
SgScopeStatement* get_scope(SgInitializedName* initname)
{
  return initname ? initname->get_scope() : NULL;
}

/// \brief returns true if func points to the main function of a
///        C, C++, or UPC program.
/// \note  recognizes UPC main functions (as opposed to SageInterface::isMain)
// \pp \todo integrate into SageInterface::isMain
bool is_main_func(const SgFunctionDefinition& func);

/// \brief builds a UPC barrier statement
// \pp \todo integrate into SageBuilder
SgUpcBarrierStatement* buildUpcBarrierStatement();

//
// helper functions
//

/// \brief replaces double quote with single quote characters in a string
std::string removeSpecialChar(std::string str);

/// \brief  finds the first parent that is an SgStatement node
/// \return the parent statement (never NULL)
SgStatement* getSurroundingStatement(SgExpression& n);

/// \overload
SgStatement* getSurroundingStatement(SgInitializedName& n);

/// \brief  determines the C++ allocation kind for type t
/// \return akCxxArrayNew if t is an array, akCxxNew otherwise
AllocKind cxxHeapAllocKind(SgType* t);

/// \brief   returns true, iff name refers to a char-array modifying function (e.g., strcpy, etc.)
bool isStringModifyingFunctionCall(const std::string& name);

/// \brief Check if a function call is a call to a function on our ignore list.
/// \details We do not want to check those functions right now. This check makes
///          sure that we dont push variables on the stack for functions that
///          we dont check and hence the generated code is cleaner
bool isGlobalFunctionOnIgnoreList(const std::string& name);

/// \brief Check if a function call is a call to library function and where
///        we check the arguments at the call site (instead of inside)
bool isLibFunctionRequiringArgCheck(const std::string& name);

/// \brief Check if a function call is a call to an IO function
bool isFileIOFunctionCall(const std::string& name);

/// \brief tests whether type is a C++ filestream (i.e., std::fstream)
bool isFileIOVariable(SgType* type);

/// \brief tests whether n was declared in a class / struct
bool isStructMember(const SgInitializedName& n);

/// \brief tests whether n is a function parameter
bool isFunctionParameter(const SgInitializedName& n);

/// \brief true, iff n is a basic block, if statement, [do]while, or for statement
bool isNormalScope( SgScopeStatement* n );

/// \brief tests whether the statement defines a global external variable
///        OR a function parameter of a function declared extern (\pp ???)
bool isGlobalExternVariable(SgStatement* stmt);

/// \brief tests whether the declaration is a constructor
/// \param mfun NULL will return false
bool isConstructor( SgMemberFunctionDeclaration* mfun );

/// \brief Follow the base type of @c type until we reach a non-typedef, non-reference.
SgType* skip_ReferencesAndTypedefs( SgType* type );

/// \brief returns the t's base type, iff t is a reference;
///        t otherwise
SgType* skip_References( SgType* t );

/// \brief   returns the base type for arrays and pointers
/// \param t a type
/// \return  the base_type, if it exists; t otherwise
/// \note    type-modifiers are currently not skipped (should they be?)
///          e.g., int* volatile X[] = /* ... */;
SgType* skip_ArrPtrType(SgType* t);

/// \brief   returns an array's base type
/// \param t a type
/// \return  the base_type, if it exists; t otherwise
SgType* skip_ArrayType(SgType* t);

/// \brief  skips one modifier type node
/// \return the base type if t is an SgModifierType
///         t otherwise
SgType* skip_ModifierType(SgType* t);

/// \brief Follow the base type of @c type until we reach a non-typedef.
SgType* skip_Typedefs( SgType* type );

/// \brief skips all references and modifiers on top
SgType* skip_TopLevelTypes( SgType* type );

/// skips potential typedefs and references and returns the array underneath
SgReferenceType* isUsableAsSgReferenceType( SgType* type );

/// skips potential modifiers and returns the reference
SgArrayType* isUsableAsSgArrayType( SgType* type );

/// \brief skips a pointer type and derivatives (typedefs and modifiers if they
///        belong to a pointer type)
// SgType* skip_PointerLevel(SgType* t);

/// finds a pointer types under top level types (such as modifiers, references)
SgPointerType* discover_PointerType(SgType* t);

/// \brief checks if varRef is part of stmt
/// \deprecated
/// \todo replace with isAncestorOf
bool traverseAllChildrenAndFind(SgExpression* varRef, SgStatement* stmt);

/// \brief checks if initName is part of stmt
/// \deprecated
/// \todo replace with isAncestorOf
bool traverseAllChildrenAndFind(SgInitializedName* initName, SgStatement* stmt);

/// \brief  converts the parent to a basic block (unless it already is one)
/// \return a SgBasicBlock object
/// \note   compared to SageBuilder::ensureParentIsBasicBlock, this function
///         requires that stmt appears in a context that allows its
///         conversion to a SgBasicBlock node.
SgBasicBlock& requiresParentIsBasicBlock(SgStatement& stmt);

/// \brief returns the UPC blocksize of types
///        if the type is not a upc shared type the returned blocksize is 0.
long upcBlocksize(const SgType* n);

/// \brief returns true, iff n is a shared type
///        e.g., shared int a;
///              shared[] int x[2][3][4];
///            *but not*
///              shared int* p;
bool isUpcShared(const SgType* n);

/// \brief returns true, iff t is a UPC shared pointer
///        e.g., shared int* shared p;
///            *but not*
///              shared int* p;
bool isUpcSharedPointer(SgType* t);

/// \brief returns the region where the variable n is allocated
///        either akStack, akGlobal, or akUpcSharedGlobal
AllocKind varAllocKind(const SgInitializedName& n);

/// appends the classname
void appendClassName( SgExprListExp* arg_list, SgType* type );

/// appends a boolean value
void appendBool( SgExprListExp* arg_list, bool b );

/// implemented in support
SgExpression* getExprBelowAssignment(SgExpression* exp);

//
// helper functions to insert rted checks

enum InsertLoc { ilAfter = 0, ilBefore = 1 };

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

/// \brief   moves the body of a function f to a new function f`;
///          f's body is replaced with code that forwards the call to f`.
/// \return  a pair indicating the statement containing the call of f`
///          and an initialized name refering to the temporary variable
///          holding the result of f`. In case f returns void
///          the initialized name is NULL.
/// \param   definingDeclaration the defining function declaration of f
/// \param   newName the name of function f`
/// \pre     definingDeclaration must be a defining declaration of a
///          free standing C/C++ function.
///          typeid(SgFunctionDeclaration) == typeid(definingDeclaration)
///          i.e., this function is NOT implemented for class member functions,
///          template functions, procedures, etc.
/// \details f's new body becomes { f`(...) } and { int res = f`(...); return res; }
///          for functions returning void and a value, respectively.
///          two function declarations are inserted in f's enclosing scope
///          result_type f`(...);                       <--- (1)
///          result_type f (...) { forward call to f` }
///          result_type f`(...) { original code }      <--- (2)
///          Calls to f are not updated, thus in the transformed code all
///          calls will continue calling f (this is also true for
///          recursive function calls from within the body of f`).
///          After the function has created the wrapper,
///          definingDeclaration becomes the wrapper function
///          The definition of f` is the next entry in the
///          statement list; the forward declaration of f` is the previous
///          entry in the statement list.
/// \todo    move to SageInterface
/// \todo    are the returned values the most meaningful?
///          maybe it is better to return the call statement of the original
///          implementation?
std::pair<SgStatement*, SgInitializedName*>
wrapFunction(SgFunctionDeclaration& definingDeclaration, SgName newName);

/// \overload
/// \tparam  functor that generates a new name based on the old name.
///          interface: SgName @nameGen(const SgName&)
/// \param   nameGen name generator
/// \todo    move to SageInterface
template <class NameGen>
std::pair<SgStatement*, SgInitializedName*>
wrapFunction(SgFunctionDeclaration& definingDeclaration, NameGen nameGen)
{
  return wrapFunction(definingDeclaration, nameGen(definingDeclaration.get_name()));
}

typedef std::vector<SgMemberFunctionDeclaration*> SgMemberFunctionDeclarationPtrList;

/// \brief Appends all of the constructors of @c type to @c constructors. The
///        constructors are those member functions whose name matches that of
///        the type.
void appendConstructors(SgClassDefinition* cdef, SgMemberFunctionDeclarationPtrList& constructors);

/* -----------------------------------------------------------
 * tps : 6March 2009: This class adds transformations
 * so that runtime errors are caught at runtime before they happen
 * -----------------------------------------------------------*/

typedef std::pair<SgReturnStmt*, size_t> ReturnInfo;

class RtedTransformation
{
   typedef std::map<SgVarRefExp*,std::pair< SgInitializedName*, AllocKind> > InitializedVarMap;

public:
   enum ReadWriteMask { Read = 1, Write = 2, BoundsCheck = 4 };

private:
   // \pp added an enum to give names to what were integer values before.
   //     Whole      ... affected size is the whole object
   //     Elem       ... affected size is an array element
   enum AppendKind { Whole = 0, Elem = 2 };

   // track the files that we're transforming, so we can ignore nodes and
   // references to nodes in other files

public:
   typedef std::vector< std::pair<SgExpression*, AllocKind> >        Deallocations;
   typedef std::vector<SgScopeStatement*>                            ScopeContainer;
   typedef std::map<SgSourceFile*, SgNamespaceDeclarationStatement*> SourceFileRoseNMType;
   typedef std::vector<SgPointerDerefExp*>                           SharedPtrDerefContainer;
   typedef std::vector<SgFunctionCallExp*>                           CallSiteContainer;

   RtedSymbols                   symbols;
   std::vector< SgSourceFile* >  srcfiles;

private:
   RtedFiles                     rtedfiles;

   // VARIABLES ------------------------------------------------------------
   // ------------------------ array ------------------------------------
   /// The array of callArray calls that need to be inserted
   std::map<SgVarRefExp*, RtedArray>        create_array_define_varRef_multiArray;
   std::map<SgPntrArrRefExp*, RtedArray>    create_array_access_call;

   /// remember variables that were used to create an array. These cant be reused for array usage calls
   std::vector<SgVarRefExp*>                variablesUsedForArray;

public:
   /// stores deref expressions of shared pointers
   SharedPtrDerefContainer                  sharedptr_derefs;

   /// stores call sites that need to be instrumented
   CallSiteContainer                        callsites;

private:
   /// this vector is used to check which variables have been marked as initialized (through assignment)
   InitializedVarMap                        variableIsInitialized;

public:
   /// the following stores all variables that are created (and used e.g. in functions)
   /// We need to store the name, type and initialized value
   /// We need to store the variables that are being accessed
   std::map<SgInitializedName*, RtedArray>  create_array_define_varRef_multiArray_stack;
   std::vector<SgVarRefExp*>                variable_access_varref;
   std::vector<SgInitializedName*>          variable_declarations;
   std::vector<SgFunctionDefinition*>       function_definitions;

   /// function calls to free
   Deallocations                            frees;

   /// return statements that need to be changed
   std::vector<ReturnInfo>                  returnstmt;

   /// Track pointer arithmetic, e.g. ++, --
   std::vector< SgExpression* >             pointer_movements;
private:
   /// map of expr ϵ { SgPointerDerefExp, SgArrowExp }, SgVarRefExp pairs
   /// the deref expression must be an ancestor of the varref
   std::map<SgPointerDerefExp*,SgVarRefExp*> variable_access_pointerderef;

   /// The second SgExpression can contain either SgVarRefExp,
   /// or a SgThisExp
   std::map<SgArrowExp*,   SgVarRefExp*>    variable_access_arrowexp;
   std::map<SgExpression*, SgThisExp*>      variable_access_arrowthisexp;

   // ------------------------ string -----------------------------------
   /// handle call to functioncall
   std::vector<RtedArguments>               function_call;

   /// calls to functions whose definitions we don't know, and thus, whose
   std::vector<SgFunctionCallExp*>          function_call_missing_def;

   ///   signatures we must check at runtime function calls to realloc
   std::vector<SgFunctionCallExp*>          reallocs;

public:
   /// what statements we need to bracket with enter/exit scope calls
   ScopeContainer                                    scopes;

   /// store all classdefinitions found
   std::map<SgClassDefinition*,RtedClassDefinition*> class_definitions;

   /// indicates if we have a globalconstructor build or not
   SgClassDeclaration*                               globConstructor;
   SgBasicBlock*                                     globalFunction;
   SgVariableDeclaration*                            globalConstructorVariable;

   SgBasicBlock* buildGlobalConstructor(SgScopeStatement* scope, std::string name);
   SgBasicBlock* appendToGlobalConstructor(SgScopeStatement* scope, std::string name);
   void appendGlobalConstructor(SgScopeStatement* scope, SgStatement* stmt);
   void appendGlobalConstructorVariable(SgScopeStatement* scope, SgStatement* stmt);
   SgVariableDeclaration* getGlobalVariableForClass(SgGlobal* globel, SgClassDeclaration* classStmt);


   // The following are vars that are needed for transformations
   // and retrieved through the visit function
   SgStatement*                                      mainFirst;
   SgStatement*                                      globalsInitLoc;
   SgBasicBlock*                                     mainBody;
   SourceFileRoseNMType                              sourceFileRoseNamespaceMap;

   // FUNCTIONS ------------------------------------------------------------
   // Helper function

   /// Transformation specific Helper Functions
   /// Returns the defining definition for the function called by fn_call, if
   /// possible.  If the direct link does not exist, will do a memory pool
   /// traversal to find the definition.  May still return NULL if the definition
   /// cannot be determined statically.
   SgFunctionDeclaration* getDefiningDeclaration( SgFunctionCallExp* fn_call );

   void insertAssertFunctionSignature( SgFunctionCallExp* exp );
   void insertConfirmFunctionSignature( SgFunctionDefinition* fndef );
   void insertFreeCall(SgExpression* freeExp, AllocKind ak);
   void insertReallocateCall( SgFunctionCallExp* exp );

public:
   bool isInInstrumentedFile( SgNode* n );
   void visit_isArraySgAssignOp(SgAssignOp* const);

   void appendFileInfo( SgExprListExp* arg_list, SgStatement* stmt);
   void appendFileInfo( SgExprListExp* arg_list, SgScopeStatement* scope, Sg_File_Info* n);

   /// appends the allocation kind
   void appendAllocKind( SgExprListExp* arg_list, AllocKind kind );

   /// appends a function signature (typecount, returntype, arg1, ... argn)
   /// to the argument list.
   void appendSignature( SgExprListExp* arg_list, SgType* return_type, const SgTypePtrList& param_types);
private:


   // ********************* Deep copy classes in headers into source **********
   SgClassDeclaration* instrumentClassDeclarationIntoTopOfAllSourceFiles(SgProject* project, SgClassDeclaration* classDecl);

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

    SgCastExp* ctorDimensionList(SgAggregateInitializer* exp) const;

    /// \brief   creates an address descriptor
    SgAggregateInitializer* mkAddressDesc(AddressDesc desc) const;

    /// \brief   creates a Sage representation of ak
    SgEnumVal* mkAllocKind(AllocKind ak) const;

    /// \brief     creates an expression constructing an rted_address
    /// \param exp an expression that will be converted into an address
    /// \param upcShared indicates whether the address is part of the PGAS shared space
    SgFunctionCallExp* mkAddress(SgExpression* exp, bool upcShared) const;

    /// \brief returns the canonical pointer to the rted_TypeDesc type
    SgType* roseTypeDesc() const    { return symbols.roseTypeDesc; }

    /// \brief returns the canonical pointer to the rted_AddressDesc type
    SgType* roseAddressDesc() const { return symbols.roseAddressDesc; }

    /// \brief returns the canonical pointer to the rted_FileInfo type
    SgType* roseFileInfo() const    { return symbols.roseSourceInfo; }

    /// \brief returns the RTED representation type for array dimensions
    SgType* roseDimensionType() const { return symbols.size_t_member; }

    /// \brief returns the RTED representation type for char*
    SgType* roseConstCharPtrType() const
    {
      return SageBuilder::buildPointerType( SageBuilder::buildConstType(SageBuilder::buildCharType()) );
    }

public:
   /// \brief rewrites the last statement in main (see member variable mainLast)
   void insertMainCloseCall();

   void visit_isAssignInitializer(SgAssignInitializer* const n);
   void visit_isArrayPntrArrRefExp(SgPntrArrRefExp* const n);

   void addPaddingToAllocatedMemory(SgStatement* stmt, const RtedArray& array);

   // Function that inserts call to array : runtimeSystem->callArray
   void insertArrayCreateCall(const RtedArray& value, SgVarRefExp* n);
   void insertArrayCreateCall(const RtedArray& value);
   void insertArrayCreateCall(SgExpression* const srcexp, const RtedArray& value);
   SgStatement* buildArrayCreateCall(SgExpression* const src_exp, const RtedArray& array, SgStatement* const stmt);

   void insertArrayAccessCall(SgPntrArrRefExp* arrayExp, const RtedArray& value);
   void insertArrayAccessCall(SgStatement* stmt, SgPntrArrRefExp* arrayExp, const RtedArray& array);

   bool isVarRefInCreateArray(SgInitializedName* search);
   void insertFuncCall(RtedArguments& args);
   void insertIOFuncCall(RtedArguments& args);
   void visit_isFunctionCall(SgFunctionCallExp* const fcexp);

public:
   /// Insert calls to registerPointerChange.  Don't worry about checkMemReads,
   /// those should be handled elsewhere (i.e. varref), but after the assignment,
   /// even if the memory was readable, ensure we stayed within array bounds.
   void insert_pointer_change( SgExpression* op );
private:

   /// \brief converts an expression yielding an integer to a char*
   /// \note  used to pass unstructured arguments to the runtime-system
   SgFunctionCallExp* convertIntToString(SgExpression* i);

   // simple scope handling
   void bracketWithScopeEnterExit( SgScopeStatement* stmt_or_block, Sg_File_Info* exit_file_info );


   // is it a variable?
   void insertCreateObjectCall( RtedClassDefinition* cdef );
   void insertVariableCreateCall(SgInitializedName* initName);
   void insertInitializeVariable(SgInitializedName*, SgVarRefExp*, AllocKind);
   SgExpression* buildVariableInitCallExpr(SgInitializedName*, SgVarRefExp*, SgStatement*, AllocKind);
   SgFunctionCallExp* buildVariableCreateCallExpr(SgInitializedName* name, bool forceinit=false);
   // TODO 2 djh: test docs
   /**
    * @b{ For Internal Use Only }.  See the overloaded convenience functions.
    */
   SgFunctionCallExp* buildVariableCreateCallExpr( SgVarRefExp* var_ref, const std::string& debug_name, bool init );

   SgExprStatement* buildVariableCreateCallStmt( SgInitializedName* name, bool isparam=false );

   void insertVariableCreateInitForParams( SgFunctionDefinition* n);
   void insertAccessVariable(SgVarRefExp* varRefE,SgExpression* derefExp);
   void insertAccessVariable(SgThisExp* varRefE,SgExpression* derefExp);
   void insertAccessVariable(SgScopeStatement* scope, SgExpression* derefExp, SgStatement* stmt, SgExpression* varRefE);
   void addFileIOFunctionCall(SgVarRefExp* n, bool read);
   void insertCheckIfThisNull(SgThisExp* texp);

public:
   void visit_isSgVarRefExp(SgVarRefExp* n, bool isRightBranchOfBinaryOp, bool thinkItsStopSearch);
   void visit_isSgArrowExp(SgArrowExp* const n);
   void visit_isSgPointerDerefExp(SgPointerDerefExp* const);
private:
   /// Renames the original main function
   /// copied from projects/UpcTranslation/upc_translation.C
   void renameMain(SgFunctionDefinition& sg_func);
   void changeReturnStmt(ReturnInfo rstmt);
   void insertExitBlock(SgStatement& stmt, size_t openblocks);

   /// builds a call to rtedExitBlock
   /// \param blocks number of blocks to close (e.g., for return statements, etc.)
   SgExprStatement* buildExitBlockStmt(size_t blocks, SgScopeStatement*, Sg_File_Info*);

   /// builds a call to rtedEnterBlock
   /// \param scopename name helps users debugging
   SgExprStatement* buildEnterBlockStmt(const std::string& scopename);

   /// factors commonalities of heap allocations
   void arrayHeapAlloc(SgInitializedName*, SgVarRefExp*, SgExpression*, AllocKind);

   /// creates a heap array record for a single argument allocation (e.g., malloc)
   void arrayHeapAlloc1(SgInitializedName*, SgVarRefExp*, SgExpressionPtrList&, AllocKind);

   /// creates a heap array record for a two argument allocation (e.g., calloc)
   void arrayHeapAlloc2(SgInitializedName*, SgVarRefExp*, SgExpressionPtrList&, AllocKind);

   AllocKind arrayAllocCall(SgInitializedName*, SgVarRefExp*, SgExprListExp*, SgFunctionDeclaration*, AllocKind);
   AllocKind arrayAllocCall(SgInitializedName*, SgVarRefExp*, SgExprListExp*, SgFunctionRefExp*, AllocKind);

   //
   // upc additions
public:
   typedef std::vector<SgStatement*>    UpcBlockingOpsContainer;

   UpcBlockingOpsContainer upcBlockingOps;
   RtedOptions             options;

public:

   RtedTransformation(const RtedFiles& prjfiles, const RtedOptions& cmdlineOpt)
   : symbols(),
     srcfiles(),
     rtedfiles(prjfiles),
     create_array_define_varRef_multiArray(),
     create_array_access_call(),
     variablesUsedForArray(),
     sharedptr_derefs(),
     callsites(),
     variableIsInitialized(),
     create_array_define_varRef_multiArray_stack(),
     variable_access_varref(),
     variable_declarations(),
     function_definitions(),
     frees(),
     returnstmt(),
     pointer_movements(),
     variable_access_pointerderef(),
     variable_access_arrowexp(),
     variable_access_arrowthisexp(),
     function_call(),
     function_call_missing_def(),
     reallocs(),
     scopes(),
     globConstructor(false),
     globalFunction(NULL),
     globalConstructorVariable(NULL),
     mainFirst(NULL),
     globalsInitLoc(NULL),
     mainBody(NULL),
     sourceFileRoseNamespaceMap(),
     classesInRTEDNamespace(),
     upcBlockingOps(),
     options(cmdlineOpt)
   {}


   /// \brief analyse project and apply necessary transformations
   void transform(SgProject* project);

   /// \brief Run frontend and return project
   SgProject* parse(int argc, char** argv);

   /// \brief Looks up RTED symbols in the given source file (needed for transformations)
   void loadFunctionSymbols(SgSourceFile& n, SourceFileType sft);

   SgAggregateInitializer* mkTypeInformation(SgType* type, bool resolve_class_names, bool array_to_pointer);

   /// \brief appends the array dimensions to the argument list
   void appendDimensions(SgExprListExp* arg_list, const RtedArray&);

   /// \brief appends the array dimensions to the argument list if needed
   ///        (i.e., rce is a RtedClassArrayElement)
   void appendDimensionsIfNeeded(SgExprListExp* arg_list, RtedClassElement* rce);

   void appendAddressAndSize(SgExprListExp* arg_list, AppendKind ak, SgScopeStatement* scope, SgExpression* varRef, SgClassDefinition* cd);
   void appendAddressAndSize(SgExprListExp* arg_list, AppendKind ak, SgExpression* exp, SgType* type, SgClassDefinition* isUnionClass);

   /// \brief generates an address for exp; If exp is ++, +=, -- or -=,
   ///        the address is taken from the pointer after the update
   SgFunctionCallExp* genAdjustedAddressOf(SgExpression* exp);

   /// \brief appends the address of exp to the arg_list
   /// \note  see also genAdjustedAddressOf for a description
   ///        on how the address is generated
   void appendAddress( SgExprListExp* arg_list, SgExpression* exp );

   /**
    * Handle instrumenting function calls in for initializer statements, which may
    * contain variable declarations.  The basic approach is to instead add the
    * function calls to the test, and ensuring that:
    *
    *     - The original test's truth value is used as the truth value of the
    *       new expression.
    *     - The instrumented function calls are invoked only once.
    *
    *   Note that this will only work for function calls that return a value
    *   suitable for bitwise operations.
    *
    *   @param  exp       An expression, which must be a legal operand to a
    *                     bitwise operator.  It will be added to the for loop's
    *                     test in a way to make it as semantically equivalent as
    *                     possible as adding it to the initializer statement.
    *
    *   @param  for_stmt  The for statement to add @c exp to.
    */
   void prependPseudoForInitializerExpression( SgExpression* exp, SgStatement* for_stmt );

   void insertRegisterTypeCall(RtedClassDefinition* const rtedClass);
   void visit_isClassDefinition(SgClassDefinition* const cdef);

   void executeTransformations();
   void insertNamespaceIntoSourceFile(SgSourceFile* sf);
   void insertNamespaceIntoSourceFile(SgProject* project);
   // void insertNamespaceIntoSourceFile(SgProject* project, std::vector<SgClassDeclaration*>&);

   void populateDimensions( RtedArray& array, SgInitializedName& init, SgArrayType& type );
   void transformIfMain(SgFunctionDefinition&);

   //
   // dependencies on AstSimpleProcessing
   //   (see also comment in RtedTransformation.cpp)
   // virtual void visit(SgNode* n); // needed for the class extraction

   //
   // implemented in RtedTransf_Upc.cpp
   void transformUpcBlockingOps(SgStatement* stmt);

   /// wraps UPC shared ptr to shared derefs by a lock
   ///   to guarantee consistency
   void transformPtrDerefs(SharedPtrDerefContainer::value_type ptrderef);

   /// wraps function calls in beginScope / endScope
   void transformCallSites(CallSiteContainer::value_type callexp);

   /// \brief transforms a UPC barrier statement
   // void transformUpcBarriers(SgUpcBarrierStatement* stmt);
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
