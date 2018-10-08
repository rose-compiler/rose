#ifndef ARITHCHECK_H
#define ARITHCHECK_H
//#include "CheckBase.h"
#include <cstddef>
#include <cstdlib>
#include <stdio.h>

#include "staticSingleAssignment.h"
#include "funcDecl.h"
#include "traverse.h"

//#include "ControlFlowGraph.h"

//#include "rose.h"
#include <vector>



#if 0
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

enum Util::OVL_TYPE {
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

typedef Rose_STL_Container<class FuncInfo*> FuncInfoList;


class Util::nodeType {
    public:
    Util::nodeType():attr(UNKNOWN){}
    Util::nodeType(enum Attr newattr): attr(newattr){}
    Util::nodeType(enum Attr newattr, enum InhAttr new_inhattr): attr(newattr), inhattr(new_inhattr){}
    
    enum Attr attr;
    enum InhAttr inhattr;

    void set_attr(enum Attr input) {
        attr = input;
    }

    void set_inh_attr(enum InhAttr input) {
        inhattr = input;
    }

};
#endif

#if 0
class Util::nodeType {
    public:
    Util::nodeType():type(NULL), node(NULL) {
    }
    Util::nodeType(SgType* ret) : type(ret), node(NULL) {
    }
    Util::nodeType(SgNode* temp) : type(NULL), node(temp) {
    }
    Util::nodeType(SgType* ret, SgNode* temp) : type(ret), node(temp) {
    }

    SgType* type;
    SgNode* node; // for debugging
};
#endif


class typeTracking : public AstBottomUpProcessing<Util::nodeType> {
    protected:
    Util::nodeType virtual evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList synList);
};

class derefTypeTracking : public AstBottomUpProcessing<Util::nodeType> {
    protected:
    Util::nodeType virtual evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList synList);
};

class BUTracking : public AstBottomUpProcessing<Util::nodeType> {
    protected:
    Util::nodeType virtual evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList synList);
};

class TopBotTracking : public AstTopDownBottomUpProcessing<Util::nodeType, Util::nodeType> {
    protected:
    Util::nodeType virtual evaluateSynthesizedAttribute(SgNode* node, Util::nodeType inh, 
    SynthesizedAttributesList synList);

    Util::nodeType virtual evaluateInheritedAttribute(SgNode* node, Util::nodeType inh);
};

#if 0
class TopBotTrack2 : public AstTopDownBottomUpProcessing<Util::nodeType, Util::nodeType> {
    protected:
    Util::nodeType virtual evaluateSynthesizedAttribute(SgNode* node, Util::nodeType inh, 
    SynthesizedAttributesList synList);

    Util::nodeType virtual evaluateInheritedAttribute(SgNode* node, Util::nodeType inh);
};
#endif

#if 0
class TopBotTrack3 : public AstTopDownBottomUpProcessing<Util::nodeType, Util::nodeType> {
    protected:
    Util::nodeType virtual evaluateSynthesizedAttribute(SgNode* node, Util::nodeType inh,
    SynthesizedAttributesList synList);

    Util::nodeType virtual evaluateInheritedAttribute(SgNode* node, Util::nodeType inh);
};
#endif

//class ArithCheck : public CheckBase { 
class ArithCheck {

    BopContainer SelectedBops;

    public:
    static SgProject* ACProject;

    static VariableSymbolMap_t varRemap;
    static VariableSymbolMap_t ClassStructVarRemap;
    static VariableSymbolMap_t ReverseMap;
    static SgType* created_struct_type;
    static TypeMap_t UnderlyingType;
    static ScopeTypeSet TypesInScope;
    #ifdef CPP_SCOPE_MGMT
    static Scope2TypeMap_t RelevantStructType;
    #else
    static TypeMap_t RelevantStructType;
    #endif
    static SgFunctionDeclaration* MallocWrapFnDecl;
    static TypeMap_t RelevantArgStructType;
    static NameVarMap_t VarDeclForName;
    static ExprTypeMap_t OriginalVarType;
    static ExprMap_t OriginalVarRef;
    static ExprMap_t OriginalExp;
    static ScopeLockKeyMap SLKM;

//    static NodeContainer NodesToInstrument;
    static NodeContainer DeclsToClone;
    static NodeContainer Clones;
    static SymbolMap_t CloneSymbolMap;
//    static SgNameList ForwardDecls;
//    static NameFuncInfoMap ForwardDecls;
    static FuncInfoList ForwardDecls;
    static uint64_t TravCtr;
    static StringClassMap_t TransClassDecls; 
    static Rose_STL_Container <SgClassDeclaration*> str_decls;
//    static SgDeclarationStatement* FirstDeclStmtInFile;

    bool isUseful(SgBinaryOp*);

    void process(SgBinaryOp*);
    void process(BopContainer&);

    void process(SgNode*);

    void instrument();
    void instrument(SgBinaryOp*);
    void instrument(SgNode*);
    
    void setProject(SgProject*);

       void insertPreamble();
//    void insertLibHeader();
//    void insertTimingSupport();
 
    void HackyPtrCheck7();
//    void modifyForAndWhileLoops(SgProject*);
//    void pushGlobalVarInitsToMain(SgProject* project);
//    void functionCallArgCastHandler(SgProject*);
    void cloneFunctions(SgProject* project);
    void fixCallsWithinClones(SgProject* project);
    void getStructDeclsInHeaders(SgProject* project);
//    void evictStructsFromFunctions(SgProject* project);
//    SgFunctionDeclaration* findDeclParent(SgNode* node);
//    SgClassDeclaration* duplicateStruct(SgClassDeclaration* orig, 
//                            SgName new_name, SgScopeStatement* scope, SgStatement* pos);
//    void castReturnExps(SgProject* project);
//    void replacePlusMinusAssigns(SgBinaryOp* op, bool plus=true);
//    void splitPlusMinusAssigns(SgProject* project);
//    void convertArrowThisToDot(SgProject*);

    void InstrumentNodes4(SgProject*);
//    void SimplifyNodes(SgProject*);

    SgStatement* getSuitablePrevStmt(SgStatement*);
    
    void handleVarDecls4(SgVariableDeclaration* var_decl);
    void handleVarDeclInFuncParamList(SgVariableDeclaration*);
    void handleInitNameInFuncParamList(SgInitializedName*);
    void handleVarDeclInFuncDef(SgVariableDeclaration*);

    void createVoidStruct(SgProject*);
    void createTrackingStruct(SgProject*);

    void handlePointerVars(SgVariableDeclaration*);
    void handleArrayVars(SgVariableDeclaration* var_decl);
    void handleVarRefs(SgVarRefExp*);
    void handleClassStructVarRefs(SgVarRefExp* var_ref);
    void handlePointerVarRefs(SgVarRefExp*);
    void handleArrayVarRefs(SgVarRefExp*);
    void handleVarRefs4(SgVarRefExp*);

    void handleFuncCalls4(SgFunctionCallExp*);
    void handleFuncCalls(SgFunctionCallExp*);
    
    void handlePointerDeref(SgPointerDerefExp*);
    void handleDerefs4(SgPointerDerefExp*);
    void handleDerefExp(SgPointerDerefExp*);
    void handleArrayDeref(SgPointerDerefExp*);
    SgExpression* createDerefOutputStruct(SgPointerDerefExp* ptr_deref);

    void handlePntrArrRefs4(SgPntrArrRefExp*);
    void handlePointerArrRefs(SgPntrArrRefExp*);
    void handleArrayArrRefs(SgPntrArrRefExp*);
    void handlePntrArrRefExp(SgPntrArrRefExp* array_ref);

    void handleDotExp4(SgDotExp*);
    void handlePointerDotExp(SgDotExp*);
    void handleArrayDotExp(SgDotExp*);

    void handleDotExpVarOper(SgDotExp* dot_exp);
    void handleDotExpFuncOper(SgDotExp* dot_exp); 
    void handleDotExp(SgDotExp* dot_exp);

    void handleArrowExp4(SgArrowExp*);
    void handlePointerArrowExp(SgArrowExp*);
    void handleArrayArrowExp(SgArrowExp*);
    void handleArrowExp(SgArrowExp*);
    void handleArrowExpVarOper(SgArrowExp* arrow_exp);
    void handleArrowExpFuncOper(SgArrowExp* arrow_exp);
    void handleClassStructVars(SgVariableDeclaration* var_decl);
    SgExpression* createDerefOvlForStruct(SgExpression* str);
    SgExpression* createDotArrowOverloadExp(SgBinaryOp* dotarrow_exp);
    SgExpression* handlePntrArrLHSArrowExp(SgArrowExp* arrow_exp);
    SgExpression* handleLHSArrowExp(SgArrowExp* arrow_exp);
    SgExpression* derefLHSOfArrowExp(SgArrowExp* arrow_exp);
    SgExpression* handleStructLHSArrowExp(SgArrowExp* arrow_exp);
    SgExpression* handleRHSArrowExp(SgArrowExp* arrow_exp);
    SgExpression* handlePntrArrRHSArrowExp(SgArrowExp* arrow_exp);
    std::string getStringForFn(std::string fn_name, SgType* arg1, SgType* arg2, SgType* arg3,
                                        SgType* retType);

    SgExpression* getArrayIndex(SgInitializedName* name);

    void handleCastExp4(SgCastExp*);
    void handlePointerCastExp(SgCastExp*);
    void handleArrayCastExp(SgCastExp*);
    SgExpression* handleCastExp(SgCastExp*);

    void handleAddressOfOp4(SgAddressOfOp*);
    void handlePointerAddrOfOp(SgAddressOfOp*);
    void handleArrayAddrOfOp(SgAddressOfOp*);
    

    void handleConditionals(SgBinaryOp*);
    void handleFuncDecls(SgFunctionDeclaration*);
    void handleFuncParams(SgFunctionParameterList*);
    void handleOverloadOps(SgNode*);
    void handleUnaryOverloadOps(SgNode*);
    void handleBinaryOverloadOps(SgNode*);
    SgExpression* handleAssignOverloadOps(SgNode* node);
    SgExpression* handleCSAssignOverloadOps(SgExpression* node);

    void handleAddSubtractOverloadOps(SgNode* node);
    bool isVarRefOrDotArrow(SgExpression* exp);

    SgExpression* createDerefCheck(SgExpression* ptr, SgExpression* addr, SgScopeStatement* scope,
                    SgStatement* pos);

    SgExpression* createDerefCheckWithStr(SgExpression* str, SgScopeStatement* scope,
                    SgStatement* pos);

    SgExpression* getDummyLock();
    SgExpression* getLookupTicket();
    
    SgExprListExp* makeImplicitCastsExplicitForFnCall(SgFunctionCallExp* fncall);

//    bool NoInitializer(SgVariableDeclaration* var_decl);

//    SgExpression* buildOverloadFnForOp(SgName, SgExpression*, SgType*, SgExpression* expr=NULL);
    SgScopeStatement* getScopeForVarRef(SgVarRefExp* var_ref);
    SgScopeStatement* getScopeThruParent(SgNode* node);
    SgExpression* buildPntrArrRefOverload(SgPntrArrRefExp* array_ref, SgType* retType, SgExpression* new_lhs, SgExpression* rhs);
    SgExpression* createSizeOfArg(SgNode*);
    bool ArithOrAssignOp(SgNode*);
    bool ArithOpWithSelfUpdate(SgNode* node);

//    void handleSymbolTable(SgInitializedName* orig_name, SgInitializedName* str_name);
    void handleSymbolTable(SgInitializedName* orig_name, SgInitializedName* str_name, bool addToMap = true);
    // Not part of class anymore.
//    SgInitializedName* getNameForDecl(SgVariableDeclaration* decl);
//    void handleSymbolTable(SgVariableDeclaration* orig_decl, SgVariableDeclaration* str_decl);
    void handleSymbolTable(SgVariableDeclaration* orig_decl, SgVariableDeclaration* str_decl, bool addToMap = true);
//    SgDotExp* createDotExpFor(SgVariableDeclaration* str_var_decl, std::string str_elem);
//    SgDotExp* createDotExpFor(SgInitializedName* str_var_decl, std::string str_elem);
//    SgArrowExp* createArrowExpFor(SgInitializedName* str_var_name, std::string str_elem);
//    SgName getNameForType(SgType* type);
    SgType* findInRelevantStructType(SgType* type);
//    SgType* findInUnderlyingType(SgType* type);
    bool argumentsAreStructs(SgFunctionCallExp* fncall);
    SgExprListExp* convertStructArgsToRefs(SgFunctionCallExp* fncall);
//    bool isOriginalClassType(SgType* type);
    SgInitializedName* createRefVersionFor(SgInitializedName* name);
//    SgVariableDeclaration* assignRefToLocalVar(SgInitializedName* name, SgFunctionDeclaration* fn_decl);
    SgVariableDeclaration* assignRefToLocalVar(SgInitializedName* name, SgFunctionDeclaration* fn_decl, SgScopeStatement* bb_scope);
    void addToSymbolTableAndClassRemap(SgInitializedName* orig_name, SgInitializedName* local_name);
    SgExpression* copyElementMetadata(SgInitializedName* src, SgInitializedName* dest, SgFunctionDeclaration* fn_decl);
    void addToSymbolTable(SgInitializedName* name, SgNode* parent);

//    SgVariableDeclaration* getCorrectMemberDecl(SgInitializedName* str_var_name, std::string str_elem); 
    void makeDeclStatic(SgDeclarationStatement* decl_stmt);
    void mapExprToOriginalType(SgExpression* replacement, SgType* original_type);
    SgType* findInOriginalVarType(SgExpression* replacement);

    void mapExprToOriginalVarRef(SgExpression* replacement, SgExpression* original);
    SgExpression* findInOriginalVarRef(SgExpression* replacement);
    SgExpression* skipToOriginalVarRef(SgExpression* replacement);
    SgExpression* checkInOriginalVarRef(SgExpression* replacement);
    SgVariableSymbol* checkInClassStructVarRemap(SgVariableSymbol* var_sym);

    SgVariableSymbol* checkInVarRemap(SgVariableSymbol* var_sym);

    void mapExprToOriginalExpr(SgExpression* repl, SgExpression* orig);
    SgExpression* findInOriginalExp(SgExpression* repl);
    SgExpression* checkInOriginalExp(SgExpression* repl);

    SgExpression* createArrayBoundCheckFn(SgExpression* arg1, SgExpression* arg2,
                                                SgExpression* arg3, SgType* arg4, SgScopeStatement* scope, 
                                                SgStatement* insert,
                                                bool array = false);

    SgFunctionRefExp* getOrCreateWrapperFunction(SgFunctionRefExp* fn_ref, SgExpression* arg, SgFunctionCallExp* fncall);

    void printAndCheckScopeContents(SgScopeStatement* scope, SgFunctionDeclaration* decl);
//    bool isExternal(SgName s_name);

    void handleBasicBlocks(SgBasicBlock* bb);
    void handleGlobalScope(SgGlobal* global);
    bool existsInSLKM(SgScopeStatement* scope);
    LockKeyPair findInSLKM(SgScopeStatement* scope);
    void handleNormalReturnStmts(SgReturnStmt* retstmt);
    void handleQualReturnStmts(SgReturnStmt* retstmt);

    SgType* getLockType();
    SgType* getKeyType();
    void handleClassStructReturnStmts(SgReturnStmt* retstmt);

    void insertInitCheckAtVarRef(SgVarRefExp* var_ref);
    void handleNormalVarRefs(SgVarRefExp* var_ref);

    SgExpression* createStructForVarRef(SgVarRefExp* var_ref);
    SgType* getVoidPointerType();
    SgType* getArrayIndexType();
//    SgCommaOpExp* initializeStructWith(SgVariableDeclaration* var_decl, SgVariableDeclaration* str_var_decl);
//    SgCommaOpExp* initializeStructWith(SgInitializedName* var_name, SgInitializedName* str_var_name);
    SgCommaOpExp* initializeStructWith(SgInitializedName* var_name, SgInitializedName* str_var_name, bool flip_ptr_init=false);
//    SgExpression* buildOverloadFn(SgName fn_name, SgExpression* lhs, SgExpression* rhs, SgType* retType, SgScopeStatement* scope);


//    SgExpression* buildOverloadFn(SgName fn_name, SgExpression* lhs, SgExpression* rhs, SgType* retType, SgScopeStatement* scope, SgNode* node);


//    SgExpression* build3ArgOverloadFn(SgName fn_name, SgExpression* arg1, SgExpression* arg2, SgExpression* arg3, SgType* retType, SgScopeStatement* scope);

//    SgExpression* build3ArgOverloadFn(SgName fn_name, SgExpression* arg1, SgExpression* arg2, SgExpression* arg3, SgType* retType, SgScopeStatement* scope, SgNode* node);


    //SgExpression* buildMultArgOverloadFn(SgName fn_name, SgExprListExp* parameter_list, SgType* retType, SgScopeStatement* scope, SgNode* pos);

    SgExpression* buildMultArgOverloadFn(SgName fn_name, SgExprListExp* parameter_list, SgType* retType, SgScopeStatement* scope, SgNode* pos, bool derived = false);

    SgExpression* buildAssignOverload(SgExpression* lhs, SgExpression* rhs, SgScopeStatement* scope, SgNode* pos);
    bool ScopeIsGlobal(SgNode* node);
//    SgExpression* updatePointer(SgVariableDeclaration* var_decl, SgVariableDeclaration* str_var_decl);
    SgExpression* updatePointer(SgInitializedName* var_name, SgInitializedName* str_var_name);
    SgExpression* updatePointerUsingAddr(SgVarRefExp* var_ref);

    SgVariableDeclaration* pushIndexToLocalVar(SgName arr_name, SgExpression* index_exp, SgScopeStatement* scope);
    SgExpression* createBoundsCheckFn(SgInitializedName* str_array_name, SgExpression* index_exp, SgScopeStatement* scope);

    SgVariableSymbol* findSymbolForVarRef(SgVarRefExp* exp);
    SgVariableSymbol* findInReverseMap(SgVariableSymbol* var_symbol);
    SgName getFunctionName(SgFunctionCallExp* fncall);
//    SgScopeStatement* getScopeForExp(SgExpression* exp);

    SgInitializedName* getNameForSymbol(SgVariableSymbol* var_symbol);

    SgCommaOpExp* copyVarInitAndUpdate(SgVariableDeclaration* var_decl, 
                            SgVariableDeclaration* str_var_decl);

    void updateMetaDataAndReassignAddr(SgInitializedName* str, 
                                            SgInitializedName* orig, 
                                            SgFunctionDeclaration* func_decl);

    bool findInVarRemap(SgVarRefExp* var_ref);

//    SgVariableDeclaration* createLocalVariable(SgInitializedName* orig);
//    SgVariableDeclaration* createLocalVariable(SgName name, SgType* type, SgExpression* init, SgScopeStatement* scope);
//    SgAddressOfOp* createAddressOfOpFor(SgInitializedName* name);
//    SgAddressOfOp* createAddressOfOpFor(SgExpression* exp);
    //SgAddressOfOp* createAddressOfOpFor(SgVarRefExp* exp);
//    SgExpression* createStructUsingAddressOf(SgExpression* exp);
    SgExpression* createStructUsingAddressOf(SgExpression* exp, SgNode* pos);

    SgExpression* getArrayIndex(SgExpression* exp);
    SgType* getArrayBaseType(SgExpression* exp);
    //SgVariableDeclaration* createStructVariableFor(SgVariableDeclaration* var_decl);
    SgVariableDeclaration* createStructVariableFor(SgVariableDeclaration* var_decl, bool array_var=false);
    SgInitializedName* createStructVariableFor(SgInitializedName*);
//    SgVariableDeclaration* createStructVariableForArrayVar(SgVariableDeclaration* var_decl);
    void createLocalVariableAndUpdateMetadata(SgInitializedName*, SgInitializedName*, SgFunctionDeclaration*);
//    SgType* createStruct(SgType* ptr_type, SgNode* node);
    // This has been moved out of the class and is now a separate function by itself
//    std::string generateNameForType(SgType* ptr_type);

    SgType* checkStructDeclsInHdr(SgType* type);
//    SgType* getStructType(SgType*, SgNode* node, bool create=false);
    SgStatement* findInsertLocation(SgNode* node);
    SgStatement* getInsertLocation(SgNode* node, SgScopeStatement* scope);
    SgStatement* findInsertLocationFromParams(SgExprListExp* param_list);
    SgStatement* getFirstStatementInScope(SgScopeStatement* scope);
    void insertStmtBeforeFirst(SgStatement* stmt, SgScopeStatement* scope);
    void insertStmtAfterLast(SgStatement* stmt, SgScopeStatement* scope);
    SgStatement* getLastStatementInScope(SgScopeStatement* scope);

    SgExpression* insertPushCall(SgExpression*, SgExpression*);
    void unwrapStructAndPush(SgStatement* retstmt);

    void insertForwardDecl(SgType* cls_type, SgScopeStatement* scope);
    SgType* getUnderlyingTypeForStruct(SgType* str_type);
    SgType* getTypeForPntrArr(SgType* type);
    SgBasicBlock* getFuncBody(SgFunctionDefinition* fndef);
    SgFunctionDefinition* getFuncDef(SgFunctionDeclaration* fndecl);
    SgFunctionDeclaration* getFuncDeclForNode(SgNode* node);
    void handleReturnStmts(SgReturnStmt* retstmt);

//    bool isValidStructType(SgType*);
    SgName getStringFor(SgType*, SgType*, SgUnaryOp*);

    SgExpression* stripDeref(SgExpression* exp);

    bool isClassStructMemberVar(SgVariableDeclaration* var_decl);
    SgExpression* buildCreateEntryFnForArrays(SgExpression* array, SgScopeStatement* scope,
                                        SgStatement* pos);

    void insertCreateEntry(SgExpression* array, SgScopeStatement* scope, 
                                    SgStatement* pos);

    SgType* getStructTypeIfPossible(SgType*);
    SgName getOverloadingFunction(SgType*, SgType*, enum Util::OVL_TYPE);
    SgName getOverloadingFunction(SgType*, enum Util::OVL_TYPE);
    char* getStringFor(enum Util::OVL_TYPE);    
    char* getStringFor(SgBinaryOp*, enum Util::OVL_TYPE);
    #if 0
    void createForwardDeclIfNotAvailable(SgName, SgType*,
                                                SgExprListExp*,
                                                SgScopeStatement*, SgProject*);
    #endif

    SgFunctionDeclaration* getFuncDeclForName(SgInitializedName* name);


//    SgFunctionDeclaration* createRTCCopy(SgInitializedName* src, SgInitializedName* dest, SgFunctionDeclaration* fn_decl);
    void insertExecFunctions();
    void handleArgvInitialization(SgProject* project);
    void insertDummyEntries(SgProject* project);


//    void insertAtTopOfBB(SgBasicBlock* bb, SgStatement* stmt);
    void handleClassStructAssigns(SgAssignOp* assign_op);
    SgExpression* getRHSClassStruct(SgExpression* rhs);


    #if 0
    #ifdef CPP_SCOPE_MGMT
    SgFunctionSymbol* createForwardDeclIfNotAvailable(SgName, SgType*,
                        SgExprListExp*,
                        SgScopeStatement*, SgProject*, SgNode*, bool derived);
    #else
    SgName createForwardDeclIfNotAvailable(SgName, SgType*,
                        SgExprListExp*,
                        SgScopeStatement*, SgProject*, SgNode*, bool derived);
    #endif
    #endif

    void createNondefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* node, SgName s_name, bool derived);

    void createFunctionFrom(SgName s_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgName transf_name, SgNode* node, bool derived);

    SgFunctionDeclaration* createDefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                        SgScopeStatement* scope, SgProject* project, SgNode* node, bool derived, SgName s_name);

    void handleCreateStructDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                        SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);
    
//    SgExprStatement* createDotAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS);
//    SgExprStatement* createArrowAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS);

    void handleCreateStructFromAddrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                    SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);

    void handleDerefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);
    
    void handleRTCCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);

    void handlePntrArrRefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);

    void handlePtrCheckDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);
    
    void handleDerefCheckDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);
    
    void handleDerefCheckWithStrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);

    void handleAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);

    void handleCSAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);

    void handleRHSDerefAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);

    void handleDerefAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);

    void handleLHSDerefAssignAndCopyDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);


    void handleAssignDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);

    void handleDoubleDerefCheckDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);
    

    void handleIncrDecrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*, bool isDecr = false);

    void handleIncrDecrDerefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* ,bool isDecr = false);


    void appendReturnStmt(SgExpression* ret_exp, SgBasicBlock* fnbody);
    SgExpression* getCondExpOper(SgInitializedName* var_name);
    SgExpression* createConditionalExp(SgName s_name, SgExpression* lhs, SgExpression* rhs);
//    bool isCondString(SgName s_name);
    void handleConditionalDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgName s_name, SgNode* node, SgFunctionDeclaration*);

    void handleAddSubDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*, bool isSub = false);
    
    void handleAddSubDerefDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*, bool isSub = false);


    void handleCastDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);

    void handleVarRefToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project);

    void handleValueToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project);

    void handleStructToValueCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project);

    void handleStructToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
                            SgScopeStatement* scope, SgProject* project);

    void handleDerefCreateStructDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);
    
    void handleReturnPointerDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration*);


    void handleAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);
    void handleNormalAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);
    void handleStructAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node, SgFunctionDeclaration*);
    void handleBoundCheckDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                SgScopeStatement* scope, SgProject* project, SgNode* node);

    void handlePlusMinusAssignDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                                    SgScopeStatement* scope, SgProject* project, SgNode* node, 
                                    SgFunctionDeclaration* fndecl, bool minus=false);

    void handleCreateEntryWithNewLock(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
                                SgScopeStatement* scope, SgProject* project, SgNode* pos, SgFunctionDeclaration* fndecl);

//    std::string getTransfName(SgName s_name, SgType* retType, SgExprListExp* param_list);

//    bool checkIfDeclExists(SgName);
//    bool checkIfDeclExists(SgName, SgType*, SgExprListExp*);
//    void createFuncInfo(SgName s_name, SgType* retType, SgExprListExp* parameter_list);
    bool compareTypes(SgType*, SgType*);
//    bool compareNames(SgName, SgName);

    void handleThisExp(SgThisExp*);
    void handleNewExp(SgNewExp*);
    void handleDeleteExp(SgDeleteExp*);
    void handleAddressOfOp(SgAddressOfOp* aop);
    SgExpression* handleNormalVarAddressOfOp(SgAddressOfOp* aop);
    void handleStructVarAddressOfOp(SgAddressOfOp* aop);
//    SgExpression* castToAddr(SgExpression* exp);
//    SgType* getAddrType();
//    SgType* getSizeOfType();

    //void printNode(SgNode*);
    //void printNodeExt(SgNode*);
    void checkType(SgType* type);

    std::string getStringForOp(SgNode*, SgType*);


//    void simplifyDotExp(SgDotExp* dot);
//    SgVariableDeclaration* takeRefAndPushToLocalVar(SgExpression* exp);
//    SgVariableDeclaration* pushToLocalVar(SgExpression* exp, SgScopeStatement* scope);
//    void simplifyArrowExp(SgArrowExp* arrow);
//    void pushToLocalVarAndReplace(SgExpression* exp);

//    static unsigned int VarCounter;

    void forwardDecl(SgName, SgType*, SgName, SgType*, SgName arg2name, SgType* arg2Type);
};

#endif
