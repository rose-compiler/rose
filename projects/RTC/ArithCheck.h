#ifndef ARITHCHECK_H
#define ARITHCHECK_H
//#include "CheckBase.h"
#include <cstddef>
#include <cstdlib>
#include <stdio.h>

#include "rose.h"
#include <vector>

typedef std::map<SgType*, SgType*> TypeMap_t;
typedef std::map<SgExpression*, SgType*> ExprTypeMap_t;
typedef std::map<const SgVariableSymbol *, SgVariableSymbol *> VariableSymbolMap_t;
typedef std::map<SgSymbol *, SgSymbol *> SymbolMap_t;
typedef Rose_STL_Container<SgName> SgNameList;
typedef std::map<std::string, SgClassDeclaration*> StringClassMap_t;
typedef std::map<std::string, SgVariableDeclaration*> NameVarMap_t;
typedef Rose_STL_Container<SgType*> SgTypeList;
typedef std::map<SgExpression*, SgExpression*> ExprMap_t;
//typedef std::map<unsigned long long, SgExpression*> ExprMap_t;

typedef std::vector<SgBinaryOp*> BopContainer;

typedef Rose_STL_Container<SgNode*> NodeContainer;


enum Attr {
	STR_TYPE = 0,
	NOT_STR_TYPE,
	UNKNOWN
};

enum InhAttr {
	WITHIN_CLASS_DEF = 0,
	WITHIN_FN_DEF,
	WITHIN_GLOBAL,
	WITHIN_PARAMLIST,
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

typedef Rose_STL_Container<class FuncInfo*> FuncInfoList;


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

#if 0
class nodeType {
	public:
	nodeType():type(NULL), node(NULL) {
	}
	nodeType(SgType* ret) : type(ret), node(NULL) {
	}
	nodeType(SgNode* temp) : type(NULL), node(temp) {
	}
	nodeType(SgType* ret, SgNode* temp) : type(ret), node(temp) {
	}

	SgType* type;
	SgNode* node; // for debugging
};
#endif


class typeTracking : public AstBottomUpProcessing<nodeType> {
	protected:
	nodeType virtual evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList synList);
};

class derefTypeTracking : public AstBottomUpProcessing<nodeType> {
	protected:
	nodeType virtual evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList synList);
};

class BUTracking : public AstBottomUpProcessing<nodeType> {
	protected:
	nodeType virtual evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList synList);
};

class TopBotTracking : public AstTopDownBottomUpProcessing<nodeType, nodeType> {
	protected:
	nodeType virtual evaluateSynthesizedAttribute(SgNode* node, nodeType inh, 
	SynthesizedAttributesList synList);

	nodeType virtual evaluateInheritedAttribute(SgNode* node, nodeType inh);
};

class TopBotTrack2 : public AstTopDownBottomUpProcessing<nodeType, nodeType> {
	protected:
	nodeType virtual evaluateSynthesizedAttribute(SgNode* node, nodeType inh, 
	SynthesizedAttributesList synList);

	nodeType virtual evaluateInheritedAttribute(SgNode* node, nodeType inh);
};


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
	static TypeMap_t RelevantStructType;
	static SgFunctionDeclaration* MallocWrapFnDecl;
	static TypeMap_t RelevantArgStructType;
	static NameVarMap_t VarDeclForName;
	static ExprTypeMap_t OriginalVarType;
	static ExprMap_t OriginalVarRef;

	static NodeContainer NodesToInstrument;
//	static SgNameList ForwardDecls;
//	static NameFuncInfoMap ForwardDecls;
	static FuncInfoList ForwardDecls;
	static uint64_t TravCtr;
	static StringClassMap_t TransClassDecls; 

	void registerCheck();

	ArithCheck() {
		registerCheck();
	}

    bool isUseful(SgBinaryOp*);

	void process(SgBinaryOp*);
	void process(BopContainer&);

	void process(SgNode*);

	void instrument();
	void instrument(SgBinaryOp*);
	void instrument(SgNode*);
	
   	void insertPreamble();
	void insertLibHeader();
//	void insertTimingSupport();
 
	void setProject(SgProject* proj);

	void HackyPtrCheck7();
	void InstrumentNodes4(SgProject*);

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
	
	SgExprListExp* makeImplicitCastsExplicitForFnCall(SgFunctionCallExp* fncall);

	bool NoInitializer(SgVariableDeclaration* var_decl);

//	SgExpression* buildOverloadFnForOp(SgName, SgExpression*, SgType*, SgExpression* expr=NULL);
	SgScopeStatement* getScopeForVarRef(SgVarRefExp* var_ref);
	SgScopeStatement* getScopeThruParent(SgNode* node);
	SgExpression* buildPntrArrRefOverload(SgPntrArrRefExp* array_ref, SgType* retType, SgExpression* new_lhs, SgExpression* rhs);
	SgExpression* createSizeOfArg(SgNode*);
	bool ArithOrAssignOp(SgNode*);
	bool ArithOpWithSelfUpdate(SgNode* node);

//	void handleSymbolTable(SgInitializedName* orig_name, SgInitializedName* str_name);
	void handleSymbolTable(SgInitializedName* orig_name, SgInitializedName* str_name, bool addToMap = true);
	// Not part of class anymore.
//	SgInitializedName* getNameForDecl(SgVariableDeclaration* decl);
//	void handleSymbolTable(SgVariableDeclaration* orig_decl, SgVariableDeclaration* str_decl);
	void handleSymbolTable(SgVariableDeclaration* orig_decl, SgVariableDeclaration* str_decl, bool addToMap = true);
//	SgDotExp* createDotExpFor(SgVariableDeclaration* str_var_decl, std::string str_elem);
	SgDotExp* createDotExpFor(SgInitializedName* str_var_decl, std::string str_elem);
	SgArrowExp* createArrowExpFor(SgInitializedName* str_var_name, std::string str_elem);
//	SgName getNameForType(SgType* type);
	SgType* findInRelevantStructType(SgType* type);
//	SgType* findInUnderlyingType(SgType* type);
	bool argumentsAreStructs(SgFunctionCallExp* fncall);
	SgExprListExp* convertStructArgsToRefs(SgFunctionCallExp* fncall);
//	bool isOriginalClassType(SgType* type);
	SgInitializedName* createRefVersionFor(SgInitializedName* name);
//	SgVariableDeclaration* assignRefToLocalVar(SgInitializedName* name, SgFunctionDeclaration* fn_decl);
	SgVariableDeclaration* assignRefToLocalVar(SgInitializedName* name, SgFunctionDeclaration* fn_decl, SgScopeStatement* bb_scope);
	void addToSymbolTableAndClassRemap(SgInitializedName* orig_name, SgInitializedName* local_name);
	SgExpression* copyElementMetadata(SgInitializedName* src, SgInitializedName* dest, SgFunctionDeclaration* fn_decl);
	void addToSymbolTable(SgInitializedName* name, SgNode* parent);

	SgVariableDeclaration* getCorrectMemberDecl(SgInitializedName* str_var_name, std::string str_elem); 
	void makeDeclStatic(SgDeclarationStatement* decl_stmt);
	void mapExprToOriginalType(SgExpression* replacement, SgType* original_type);
	SgType* findInOriginalVarType(SgExpression* replacement);

	void mapExprToOriginalVarRef(SgExpression* replacement, SgExpression* original);
	SgExpression* findInOriginalVarRef(SgExpression* replacement);
	SgExpression* skipToOriginalVarRef(SgExpression* replacement);
	SgExpression* checkInOriginalVarRef(SgExpression* replacement);
	void printAndCheckScopeContents(SgScopeStatement* scope, SgFunctionDeclaration* decl);
	bool isExternal(SgName s_name);

	SgExpression* createStructForVarRef(SgVarRefExp* var_ref);
	SgType* getVoidPointerType();
	SgType* getArrayIndexType();
//	SgCommaOpExp* initializeStructWith(SgVariableDeclaration* var_decl, SgVariableDeclaration* str_var_decl);
//	SgCommaOpExp* initializeStructWith(SgInitializedName* var_name, SgInitializedName* str_var_name);
	SgCommaOpExp* initializeStructWith(SgInitializedName* var_name, SgInitializedName* str_var_name, bool flip_ptr_init=false);
//	SgExpression* buildOverloadFn(SgName fn_name, SgExpression* lhs, SgExpression* rhs, SgType* retType, SgScopeStatement* scope);
	SgExpression* buildOverloadFn(SgName fn_name, SgExpression* lhs, SgExpression* rhs, SgType* retType, SgScopeStatement* scope, SgNode* node);
//	SgExpression* build3ArgOverloadFn(SgName fn_name, SgExpression* arg1, SgExpression* arg2, SgExpression* arg3, SgType* retType, SgScopeStatement* scope);
	SgExpression* build3ArgOverloadFn(SgName fn_name, SgExpression* arg1, SgExpression* arg2, SgExpression* arg3, SgType* retType, SgScopeStatement* scope, SgNode* node);

	SgExpression* buildAssignOverload(SgExpression* lhs, SgExpression* rhs, SgScopeStatement* scope, SgNode* pos);
	bool ScopeIsGlobal(SgNode* node);
//	SgExpression* updatePointer(SgVariableDeclaration* var_decl, SgVariableDeclaration* str_var_decl);
	SgExpression* updatePointer(SgInitializedName* var_name, SgInitializedName* str_var_name);
	SgExpression* updatePointerUsingAddr(SgVarRefExp* var_ref);

	SgVariableDeclaration* pushIndexToLocalVar(SgName arr_name, SgExpression* index_exp, SgScopeStatement* scope);
	SgExpression* createBoundsCheckFn(SgInitializedName* str_array_name, SgExpression* index_exp, SgScopeStatement* scope);

	SgVariableSymbol* findSymbolForVarRef(SgVarRefExp* exp);
	SgVariableSymbol* findInReverseMap(SgVariableSymbol* var_symbol);
	SgName getFunctionName(SgFunctionCallExp* fncall);
	SgScopeStatement* getScopeForExp(SgExpression* exp);

	SgInitializedName* getNameForSymbol(SgVariableSymbol* var_symbol);

	SgCommaOpExp* copyVarInitAndUpdate(SgVariableDeclaration* var_decl, 
							SgVariableDeclaration* str_var_decl);

	void updateMetaDataAndReassignAddr(SgInitializedName* str, 
											SgInitializedName* orig, 
											SgFunctionDeclaration* func_decl);

	bool findInVarRemap(SgVarRefExp* var_ref);

	SgVariableDeclaration* createLocalVariable(SgInitializedName* orig);
	SgVariableDeclaration* createLocalVariable(SgName name, SgType* type, SgExpression* init, SgScopeStatement* scope);
	SgAddressOfOp* createAddressOfOpFor(SgInitializedName* name);
	SgAddressOfOp* createAddressOfOpFor(SgExpression* exp);
	//SgAddressOfOp* createAddressOfOpFor(SgVarRefExp* exp);
//	SgExpression* createStructUsingAddressOf(SgExpression* exp);
	SgExpression* createStructUsingAddressOf(SgExpression* exp, SgNode* pos);

	SgExpression* getArrayIndex(SgExpression* exp);
	SgType* getArrayBaseType(SgExpression* exp);
	//SgVariableDeclaration* createStructVariableFor(SgVariableDeclaration* var_decl);
	SgVariableDeclaration* createStructVariableFor(SgVariableDeclaration* var_decl, bool array_var=false);
	SgInitializedName* createStructVariableFor(SgInitializedName*);
//	SgVariableDeclaration* createStructVariableForArrayVar(SgVariableDeclaration* var_decl);
	void createLocalVariableAndUpdateMetadata(SgInitializedName*, SgInitializedName*, SgFunctionDeclaration*);
	SgType* createStruct(SgType* ptr_type, SgNode* node);
	std::string generateNameForType(SgType* ptr_type);
	SgType* getStructType(SgType*, SgNode* node, bool create=false);
	SgStatement* findInsertLocation(SgNode* node);
	SgStatement* getInsertLocation(SgNode* node, SgScopeStatement* scope);
	SgStatement* findInsertLocationFromParams(SgExprListExp* param_list);
	SgStatement* getFirstStatementInScope(SgScopeStatement* scope);
	void insertStmtBeforeFirst(SgStatement* stmt, SgScopeStatement* scope);
	void insertStmtAfterLast(SgStatement* stmt, SgScopeStatement* scope);
	SgStatement* getLastStatementInScope(SgScopeStatement* scope);

	void insertForwardDecl(SgType* cls_type, SgScopeStatement* scope);
	SgType* getUnderlyingTypeForStruct(SgType* str_type);
	SgType* getTypeForPntrArr(SgType* type);
	bool isDotOrArrow(SgNode* node);
	SgBasicBlock* getFuncBody(SgFunctionDefinition* fndef);
	SgFunctionDefinition* getFuncDef(SgFunctionDeclaration* fndecl);
	SgFunctionDeclaration* getFuncDeclForNode(SgNode* node);
	void handleReturnStmts(SgReturnStmt* retstmt);

//	bool isValidStructType(SgType*);
	SgName getStringFor(SgType*, SgType*, SgUnaryOp*);

	SgType* getStructTypeIfPossible(SgType*);
	SgName getOverloadingFunction(SgType*, SgType*, enum OVL_TYPE);
	SgName getOverloadingFunction(SgType*, enum OVL_TYPE);
	char* getStringFor(enum OVL_TYPE);	
	char* getStringFor(SgBinaryOp*, enum OVL_TYPE);
	#if 0
	void createForwardDeclIfNotAvailable(SgName, SgType*,
												SgExprListExp*,
												SgScopeStatement*, SgProject*);
	#endif

	SgFunctionDeclaration* getFuncDeclForName(SgInitializedName* name);


	SgFunctionDeclaration* createRTCCopy(SgInitializedName* src, SgInitializedName* dest, SgFunctionDeclaration* fn_decl);
	void insertExecFunctions();
	void insertAtTopOfBB(SgBasicBlock* bb, SgStatement* stmt);


	SgName createForwardDeclIfNotAvailable(SgName, SgType*,
						SgExprListExp*,
						SgScopeStatement*, SgProject*, SgNode*);

	void createNondefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
				SgScopeStatement* scope, SgProject* project, SgNode* node, SgName s_name);

	void createFunctionFrom(SgName s_name, SgType* retType, SgExprListExp* param_list,
				SgScopeStatement* scope, SgProject* project, SgName transf_name, SgNode* node);

	SgFunctionDeclaration* createDefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
						SgScopeStatement* scope, SgProject* project, SgNode* node);

	void handleCreateStructDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
										SgScopeStatement* scope, SgProject* project, SgNode* node);
	
	SgExprStatement* createDotAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS);
	SgExprStatement* createArrowAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS);

	void handleCreateStructFromAddrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
									SgScopeStatement* scope, SgProject* project, SgNode* node);

	void handleDerefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
								SgScopeStatement* scope, SgProject* project, SgNode* node);
	
	void handlePntrArrRefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list, 
								SgScopeStatement* scope, SgProject* project, SgNode* node);

	void handleAssignDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
								SgScopeStatement* scope, SgProject* project, SgNode* node);

	void handleIncrDecrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
								SgScopeStatement* scope, SgProject* project, SgNode* node, bool isDecr = false);
	
	void appendReturnStmt(SgExpression* ret_exp, SgBasicBlock* fnbody);
	SgExpression* getCondExpOper(SgInitializedName* var_name);
	SgExpression* createConditionalExp(SgName s_name, SgExpression* lhs, SgExpression* rhs);
	bool isCondString(SgName s_name);
	void handleConditionalDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
								SgScopeStatement* scope, SgProject* project, SgName s_name, SgNode* node);

	void handleAddSubDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
								SgScopeStatement* scope, SgProject* project, SgNode* node, bool isSub = false);

	void handleCastDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
							SgScopeStatement* scope, SgProject* project, SgNode* node);

	void handleValueToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
							SgScopeStatement* scope, SgProject* project);

	void handleStructToValueCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
							SgScopeStatement* scope, SgProject* project);

	void handleStructToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
							SgScopeStatement* scope, SgProject* project);

	void handleAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
								SgScopeStatement* scope, SgProject* project, SgNode* node);
	void handleNormalAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
								SgScopeStatement* scope, SgProject* project, SgNode* node);
	void handleStructAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
								SgScopeStatement* scope, SgProject* project, SgNode* node);


	void handlePlusMinusAssignDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
									SgScopeStatement* scope, SgProject* project, SgNode* node, bool minus=false);

	std::string getTransfName(SgName s_name, SgType* retType, SgExprListExp* param_list);

//	bool checkIfDeclExists(SgName);
	bool checkIfDeclExists(SgName, SgType*, SgExprListExp*);
	void createFuncInfo(SgName s_name, SgType* retType, SgExprListExp* parameter_list);
	bool compareTypes(SgType*, SgType*);
	bool compareNames(SgName, SgName);

	void handleAddressOfOp(SgAddressOfOp* aop);
	SgExpression* handleNormalVarAddressOfOp(SgAddressOfOp* aop);
	void handleStructVarAddressOfOp(SgAddressOfOp* aop);
	SgExpression* castToAddr(SgExpression* exp);
	SgType* getAddrType();
	SgType* getSizeOfType();

	void printNode(SgNode*);
	void printNodeExt(SgNode*);
	void checkType(SgType* type);

	std::string getStringForOp(SgNode*, SgType*);


	static unsigned int VarCounter;

	void forwardDecl(SgName, SgType*, SgName, SgType*, SgName arg2name, SgType* arg2Type);
};

#endif
