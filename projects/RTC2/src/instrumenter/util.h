#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <cstdlib>
// #include "sageGeneric.hpp"
#include "rose.h"
#include <boost/lexical_cast.hpp>

//RMM
#define RMM_MISSING_FUNC_DEFINITION_INSTRUMENTATION
//RMM


#define STRUCT_ARRAY 7
#define GENERATE_DEFS
//#define CAST_TYPE SgCastExp::e_C_style_cast
//#define COND_BOOL_RETVAL
#define CHECK_STATIC
#define CREATE_ENTRY_NOT_STATIC

// Removed because static member functions
// can't access non-static member variables. We operate
// on non-static member variables, hence the instrumentation
// functions (atleast the member functions) cannot be static
//#define MAKE_DECLS_STATIC
//#define FORWARD_CLASS_DECLS
#define ADDROP_LVAL_FALSE
//#define INLINING_CHECK
//#define IMPLICIT_CASTS
//#define HACKY_QUICK_INSERT_LOC

//#define ONLY_ADD_ARRAYS_TO_OVR

// Disabled for testing...
// FIXME: Doesn't yet support C++ constructors
//#define CLASS_STRUCT_COPY


//#define LOCK_KEY_INSERT
//#define INIT_CHECKS
#define REMOVE_TYPEDEFS_FROM_VARS
#define SKIP_MAIN_PARAMS
//#define DUMMY_LOCK 11000
#define DUMMY_LOCK 50
#define ARRAY_CREATE_ENTRY

#define DELAYED_INSTR

//#define PANIC_DEBUG

#define SKIP_TYPEDEFS_QUAL_TYPES
//#define INSERT_GLOBAL_PTR_FOR_ADDR
#define NO_INSTR_AT_VAR_REFS

//#define IN4_ITERATE_ONLY

#define DOUBLE_DIM_ENABLED
#define ARRAY_LOOKUP_USING_ADDR


#define STRIP_TYPEDEFS


#define FILE_SPECIFIC_VARNAMES

//#define CLONE_FUNCTIONS

#define ENUM_TO_INT_HACK

//#define CHECK_HDR_FILES

// Enable this to convert str args to char * args
#define STR_TO_CHARPTR_ARG_CASTS

#define EXPLICIT_RETURN_STMT_CASTS

#define INT_ARRAY_INDEX

// This fix is required to make sure that we don't cast the cout
// object to its base, basic_ostream type. This cast results in the copy
// constructor being called, and its parent class has a copy constructor that
// is private
#define COUT_FIX

// FIXME: Not completed. How to is listed in the evictStructs function
//#define EVICT_STRUCTS_FROM_FUNCTIONS

// June 8 2012: Scanner_mod.c wasn't workign when this was enabled
// since argv was recognized as an double pointer type
// rather than a pointer to an array type
#define USE_ARRAY_LOOKUP_FOR_ARGV

#define CONVERT_ARG_STRUCTS_TO_PTR

#define GENERATED_STRUCT_SIZE 2

//#define QUAL_DEBUG
//#define SYNTH_DEBUG
//#define INH_DEBUG
//#define IN4_DEBUG
//#define PNTRARRREF_DEBUG
//#define VAR_REF_DEBUG
//#define DOTEXPVAR_DEBUG
//#define FIND_INS_LOC_DEBUG
//#define ARRAY_VAR_DEBUG
#define WRAP_PTHREAD_ROUTINE_CALLS

//#define RETURN_STMT_DEBUG

#define RETAIN_FUNC_PROTOS

#define EXCLUDE_STD_NAMESP_FUNC

//#define DISCARD_GENERATED_NODES

//#define CREATE_STRUCT_VAR_REF_DEBUG
//#define GET_STRUCT_TYPE_DEBUG
//#define GENERATE_NAME_FOR_TYPE_DEBUG
//#define CREATE_STRUCT_DEBUG
//#define FUNCPARAM_DEBUG
//#define STRUCT_VAR_DEBUG
//#define CAND_FN_DEBUG
//#define FUNCCALL_DEBUG

//#define NONDEFDECL_DEBUG
//#define DEFDECL_DEBUG

//#define IDENTITY_TRANSLATOR

//#define SYNTH3_DEBUG
//#define INH3_DEBUG
//#define SN_DEBUG
//#define SIMPLIFY_EXPRESSIONS
//#define TURN_OFF_INSTR

//#define ALLOW_UNTRACKED_VARIABLES

//#define USE_INSERT_AFTER

// CPP_SCOPE_MGMT declared in ArithCheck.h
//#define UNDERLYING_TYPE_DEBUG

#define NEW_OVL_ENABLED
//#define NEW_EXP_DEBUG
#define STRIP_ARRAY_TYPE

#define DELETE_OVL_ENABLED
//#define DELETE_EXP_DEBUG

//#define STRIP_STATIC_PARAMS

#define SUPPORT_REFERENCES

#define CAST_TYPE SgCastExp::e_C_style_cast
#define GEFD(node) SageInterface::getEnclosingFunctionDeclaration(node, true)


#define CPP_SCOPE_MGMT



namespace SI = SageInterface;
namespace SB = SageBuilder;

typedef std::map<SgType*, SgType*> TypeMap_t;
typedef std::map<SgExpression*, SgType*> ExprTypeMap_t;
typedef std::map<const SgVariableSymbol *, SgVariableSymbol *> VariableSymbolMap_t;
typedef std::map<SgSymbol *, SgSymbol *> SymbolMap_t;
typedef Rose_STL_Container<SgName> SgNameList;
typedef std::map<std::string, SgClassDeclaration*> StringClassMap_t;
typedef std::map<std::string, SgVariableDeclaration*> NameVarMap_t;
typedef Rose_STL_Container<SgType*> SgTypeList;
typedef std::map<SgExpression*, SgExpression*> ExprMap_t;
typedef std::pair<SgVariableDeclaration*, SgVariableDeclaration*> LockKeyPair;
typedef std::pair<SgType*, SgType*> TypePair;
//typedef std::map<SgScopeStatement*, TypeMap_t> Scope2TypeMap_t;
typedef std::map<SgScopeStatement*, TypePair> Scope2TypeMap_t;
//typedef std::map<unsigned long long, SgExpression*> ExprMap_t;

typedef std::set<SgType*> TypeSet;
typedef std::map<SgScopeStatement*, TypeSet> ScopeTypeSet;

typedef std::vector<SgBinaryOp*> BopContainer;

typedef Rose_STL_Container<SgNode*> NodeContainer;

typedef Rose_STL_Container<class FuncInfo*> FuncInfoList;

typedef std::map<SgScopeStatement*, LockKeyPair> ScopeLockKeyMap;


namespace Util {

extern SgDeclarationStatement* FirstDeclStmtInFile;
extern SgProject* project;
extern unsigned int VarCounter;

enum Attr {
  STR_TYPE = 0,
  NOT_STR_TYPE,
  SYNTH_DOT_EXP,
  SYNTH_NOT_DOT_EXP,
  UNKNOWN
};

enum InhAttr {
  WITHIN_CLASS_DEF = 0,
  WITHIN_FN_DEF,
  WITHIN_GLOBAL,
  WITHIN_PARAMLIST,
  DOT_EXP,
  DOT_EXP_CHILD,
  INH_UNKNOWN
};

enum OVL_TYPE {
  DEREF = 0,
  CAST,
  SPECIAL_CAST,
  STRUCT,
  NORMAL
};

class FuncInfo {
  public:
  SgName fn_name;
  SgType* retType;
  SgTypeList* ParamTypes;
};




class nodeType {
  public:
  nodeType():attr(UNKNOWN){}
  nodeType(enum Attr newattr): attr(newattr){}
  nodeType(enum Attr newattr, enum InhAttr new_inhattr): attr(newattr), inhattr(new_inhattr){}

  enum Attr attr;
  enum InhAttr inhattr;

  void set_attr(enum Attr input) {
    attr = input;
  }

  void set_inh_attr(enum InhAttr input) {
    inhattr = input;
  }

};


void setProject(SgProject* proj);
SgName getNameForType(SgType* type);
bool checkIfStatic(SgDeclarationStatement* decl_stmt);
SgBasicBlock* getBBForFn(SgFunctionDeclaration* fn_decl);
SgFunctionDeclaration* getFnFor(SgNode* node);
SgDeclarationStatement* getGloballyVisibleDecl(SgNode* pos);
bool isPrimitiveDataType(SgType* type);

void printStatements(SgGlobal* global);

bool isNamespaceScope(SgScopeStatement* scope, std::string str);
bool isStdOrPrimitiveType(SgType* ty);
bool isInNamespace(SgFunctionDeclaration* scope, std::string str);


bool compareNames(SgName first, SgName second);
bool compareTypes(SgType* first, SgType* second);

bool isCondString(SgName s_name);
bool isExternal(SgName s_name);
std::string getTransfName(SgName s_name, SgType* retType, SgExprListExp* param_list);


SgVariableDeclaration* createLocalVariable(SgInitializedName* orig);
SgVariableDeclaration* createLocalVariable(SgName name, SgType* type, SgExpression* init, SgScopeStatement* scope);


SgInitializedName* getNameForDecl(SgVariableDeclaration* decl);

SgAddressOfOp* createAddressOfOpFor(SgExpression* exp);
SgAddressOfOp* createAddressOfOpFor(SgInitializedName* name);


SgExprStatement* createDotAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS);
SgExprStatement* createArrowAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS);
SgType* getVoidPointerType();
SgDotExp* createDotExpFor(SgInitializedName* str_var_name, std::string str_elem);
SgArrowExp* createArrowExpFor(SgInitializedName* str_var_name, std::string str_elem);
SgType* getSizeOfType();

SgVariableDeclaration* getCorrectMemberDecl(SgInitializedName* str_var_name, std::string str_elem);
SgExpression* castToAddr(SgExpression* exp);
SgType* getAddrType();

#if 0
SgType* findInUnderlyingType(SgType* type);
bool isValidStructType(SgType* type, SgStatement* stmt);
#endif
std::string generateNameForType(SgType* ptr_type);



SgType* skip_Typedefs(SgType* type);
SgType* getType(SgType* type);
SgExpression* castAwayTypedef(SgExpression* exp);

void printNode(SgNode* node);
void printNodeExt(SgNode* node);

bool isQualifyingType(SgType* type);
SgExprListExp* getArrayBoundCheckParams(SgExpression* arg1, SgExpression* arg2, SgExpression* arg3, SgType* arg4);
SgExprListExp* getArrayBoundCheckUsingLookupParams(SgExpression* arg1, SgExpression* arg2, SgExpression* arg3, SgType* arg4);
bool isRHSOfDotOrArrow(SgNode* node);
bool isVarRefOrDotArrow(SgExpression* exp);
bool isConditionalOp(SgNode* node);
bool isDotOrArrow(SgNode* node);

SgStatement* getSurroundingStatement(SgExpression& n);
SgStatement* getSurroundingStatement(SgNode& n);
SgType* skip_PointerType(SgType* t);
SgType* skip_ReferencesAndTypedefs( SgType* type );
SgType* resolveTypedefsAndReferencesToBaseTypes(SgType* type);
SgType* skip_RefsPointersAndTypedefs(SgType* type);
SgType* skip_ArrPntrRefTypedefs(SgType* type);
bool NoInitializer(SgVariableDeclaration* var_decl);
SgScopeStatement* getScopeForExp(SgExpression* exp);
void insertAtTopOfBB(SgBasicBlock* bb, SgStatement* stmt);
bool isFree(SgFunctionCallExp* fncall);
SgExpression* stripDeref(SgExpression* exp);
SgType* getTypeForPntrArr(SgType* type);
SgStatement* getSuitablePrevStmt(SgStatement* stmt);
SgExpression* getDummyLock();
SgExpression* getLookupTicket();
char* getStringForVariantT(VariantT var);
void makeDeclStatic(SgDeclarationStatement* decl_stmt);
SgName getFunctionName(SgFunctionCallExp* fncall);
SgFunctionDefinition* getFuncDef(SgFunctionDeclaration* fndecl);
SgBasicBlock* getFuncBody(SgFunctionDefinition* fndef);
void appendReturnStmt(SgExpression* ret_exp, SgBasicBlock* fnbody);
SgScopeStatement* getScopeForVarRef(SgVarRefExp* var_ref);
SgInitializedName* getNameForSymbol(SgVariableSymbol* var_symbol);
std::string getStringForFn(std::string fn_name, SgType* arg1, SgType* arg2, SgType* arg3,
                    SgType* retType);
std::string getStringForOp(SgNode* node, SgType* retType);
}

#endif
