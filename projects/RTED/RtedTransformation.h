#ifndef RTEDTRANS_H
#define RTEDTRANS_H

#include "RtedSymbols.h"
#include "DataStructures.h"

/* -----------------------------------------------------------
 * tps : 6March 2009: This class adds transformations
 * so that runtime errors are caught at runtime before they happen
 * -----------------------------------------------------------*/
class RtedTransformation : public AstSimpleProcessing {
 private:
  // VARIABLES ------------------------------------------------------------
  SgGlobal* globalScope;
  // ------------------------ array ------------------------------------
  // The array of callArray calls that need to be inserted
  std::map<SgVarRefExp*, RTedArray*> create_array_define_varRef_multiArray;
  std::map<SgInitializedName*, RTedArray*> create_array_define_varRef_multiArray_stack;
  std::map<SgExpression*, RTedArray*> create_array_access_call;
  // remember variables that were used to create an array. These cant be reused for array usage calls
  std::vector<SgVarRefExp*> variablesUsedForArray;
  // this vector is used to check which variables have been marked as initialized (through assignment)
  std::map<SgVarRefExp*,std::pair< SgInitializedName*,bool> > variableIsInitialized;
  // when traversing variables, we find some that are initialized names
  // instead of varrefexp, and so we create new varrefexps but we do
  // add them later and not during the same traversal.
  std::map<SgStatement*,SgStatement*> insertThisStatementLater;

  // the following stores all variables that are created (and used e.g. in functions)
  // We need to store the name, type and intialized value
  std::vector<SgInitializedName*> variable_declarations;
  // We need to store the variables that are being accessed
  std::vector<SgVarRefExp*> variable_access_varref;
  std::map<SgPointerDerefExp*,SgVarRefExp*> variable_access_pointerderef;
  // ------------------------ string -----------------------------------
  // handle call to functioncall
  std::vector<RtedArguments*> function_call;
  // function calls to free
  std::vector<SgFunctionCallExp*> frees;
  // function calls to realloc
  std::vector<SgFunctionCallExp*> reallocs;

  std::vector<SgFunctionDefinition*> function_definitions;

  // what statements we need to bracket with enter/exit scope calls
  std::vector<SgStatement*> scopes;
  // store all classdefinitions found
  std::map<SgClassDefinition*,RtedClassDefinition*> class_definitions;


  // The following are vars that are needed for transformations
  // and retrieved through the visit function
  SgFunctionSymbol* roseCreateArray;
  SgFunctionSymbol* roseArrayAccess;
  SgFunctionSymbol* roseFunctionCall;
  SgFunctionSymbol* roseFreeMemory;
  SgFunctionSymbol* roseReallocateMemory;
  SgFunctionSymbol* roseIOFunctionCall;
  SgFunctionSymbol* roseConvertIntToString;
  SgClassSymbol* runtimeClassSymbol;
  SgScopeStatement* rememberTopNode;
  SgStatement* mainLast;
  SgStatement* mainFirst;
  Sg_File_Info* mainEnd;
  SgFunctionSymbol* roseRtedClose;
  bool mainEndsWithReturn;
  SgReturnStmt* mainReturnStmt;
  SgFunctionSymbol* roseCallStack;
  SgFunctionSymbol* roseCreateVariable;
  SgFunctionSymbol* roseInitVariable;
  SgFunctionSymbol* roseAccessVariable;
  SgFunctionSymbol* roseEnterScope;
  SgFunctionSymbol* roseExitScope;
  SgFunctionSymbol* roseRegisterTypeCall;
  SgType* size_t_member;

  // FUNCTIONS ------------------------------------------------------------
  // Helper function
  SgStatement* getStatement(SgExpression* exp);
  // Transformation specific Helper Functions
  SgStatement* getSurroundingStatement(SgNode* n);
  // returns greatest lvalue expression ancestor (e.g the operand of an
  // expression statement or assign op).
  SgExpression* getUppermostLvalue( SgExpression* n );
  // insert: RuntimeSystem* runtimeSystem = new RuntimeSystem();
  void insertRuntimeSystemClass();
  void insertFreeCall( SgFunctionCallExp* exp );
  void insertReallocateCall( SgFunctionCallExp* exp );
  SgExpression* buildString(std::string name);
  std::string getMangledNameOfExpression(SgExpression* expr);
  SgExpression* getExprBelowAssignment(SgExpression* exp, int& derefCounter);
  void appendFileInfo( SgNode* n, SgExprListExp* arg_list);
  void appendFileInfo( Sg_File_Info* n, SgExprListExp* arg_list);





  // Traverse all nodes and check properties
  virtual void visit(SgNode* n);

  void insertMainCloseCall(SgStatement* main);
  void visit_checkIsMain(SgNode* n);
  void visit_isArraySgInitializedName(SgNode* n);
  void visit_isArraySgAssignOp(SgNode* n);
  void visit_isAssignInitializer(SgNode* n);

  void visit_isArrayPntrArrRefExp(SgNode* n);
  void visit_isArrayExprListExp(SgNode* n);
  void visit_isSgScopeStatement(SgNode* n);

  void addPaddingToAllocatedMemory(SgStatement* stmt,  RTedArray* array);

  // Function that inserts call to array : runtimeSystem->callArray
  void insertArrayCreateCall(SgVarRefExp* n, RTedArray* value);
  void insertArrayCreateCall(SgInitializedName* initName,  RTedArray* value);
  void insertArrayCreateCall(SgStatement* stmt,SgInitializedName* initName,  RTedArray* value);

  void insertArrayAccessCall(SgExpression* arrayExp, RTedArray* value);
  void insertArrayAccessCall(SgStatement* stmt,
			SgExpression* arrayExp, RTedArray* array);

  std::pair<SgInitializedName*,SgVarRefExp*> getRightOfDot(SgDotExp* dot , std::string str, SgVarRefExp* varRef);
  std::pair<SgInitializedName*,SgVarRefExp*> getRightOfArrow(SgArrowExp* arrow , std::string str, SgVarRefExp* varRef);
  std::pair<SgInitializedName*,SgVarRefExp*> getPlusPlusOp(SgPlusPlusOp* plus ,std::string str, SgVarRefExp* varRef);
  std::pair<SgInitializedName*,SgVarRefExp*> getMinusMinusOp(SgMinusMinusOp* minus ,std::string str, SgVarRefExp* varRef);
  std::pair<SgInitializedName*,SgVarRefExp*> getRightOfPointerDeref(SgPointerDerefExp* dot, std::string str, SgVarRefExp* varRef);
 
  int getDimension(SgInitializedName* initName);
  int getDimension(SgInitializedName* initName,SgVarRefExp* varRef);
  SgVarRefExp* resolveToVarRefRight(SgExpression* expr);
  SgVarRefExp* resolveToVarRefLeft(SgExpression* expr);
  RtedSymbols* symbols;

  bool isVarRefInCreateArray(SgInitializedName* search);
  void insertFuncCall(RtedArguments* args);
  void insertIOFuncCall(RtedArguments* args);
  void insertStackCall(RtedArguments* args);
  void insertStackCall(RtedArguments* args, bool before);
  void visit_isFunctionCall(SgNode* n);
  void visit_isFunctionDefinition(SgNode* n);
  bool isStringModifyingFunctionCall(std::string name);
  int getDimensionForFuncCall(std::string name);
  bool isFunctionCallOnIgnoreList(std::string name);
  bool isFileIOFunctionCall(std::string name) ;
  SgExpression* getVariableLeftOfAssignmentFromChildOnRight(SgNode* n);

  // simple scope handling
  std::string scope_name( SgStatement* n);
  void bracketWithScopeEnterExit( SgStatement* n);


  // is it a variable?
  void visit_isSgVariableDeclaration(SgNode* n);
  void insertVariableCreateCall(SgInitializedName* initName);
  bool isVarInCreatedVariables(SgInitializedName* n);
  void insertInitializeVariable(SgInitializedName* initName,
				SgVarRefExp* varRefE, bool ismalloc 
				);
  SgExprStatement* buildVariableCreateCallStmt(SgInitializedName* name, SgStatement* stmt, bool forceinit=false);
  void insertVariableCreateInitForParams( SgFunctionDefinition* n);
  void insertAccessVariable(SgVarRefExp* varRefE,SgPointerDerefExp* derefExp);
  void visit_isSgVarRefExp(SgVarRefExp* n);

  std::string removeSpecialChar(std::string str);

 public:
  RtedTransformation() {
    //inputFiles=files;
    globalScope=NULL;
    roseCreateArray=NULL;
    roseArrayAccess=NULL;
    roseRtedClose=NULL;
    roseFunctionCall=NULL;
    roseConvertIntToString=NULL;
    roseCallStack=NULL;
    symbols = new RtedSymbols();
    mainFirst=NULL;
    mainEnd=NULL;
    mainLast=NULL;
    mainEndsWithReturn=false;
    roseCreateVariable=NULL;
    roseInitVariable=NULL;
    roseAccessVariable=NULL;

  };
  virtual ~RtedTransformation(){


  };

  // PUBLIC FUNCTIONS ------------------------------------------------------------
  // Insert Header Files
  void insertProlog(SgProject* proj);

  // analyse file and apply necessary (call) transformations
  void transform(SgProject* project);

  // Run frontend and return project
  SgProject* parse(int argc, char** argv);

  void appendAddressAndSize(SgInitializedName* initName, SgExpression* varRef, SgStatement* stmt, SgExprListExp* arg_list, int appendType);

  bool isGlobalExternVariable(SgStatement* stmt);

  void insertRegisterTypeCall(RtedClassDefinition* rtedClass);
  void visit_isClassDefinition(SgClassDefinition* cdef);


};

#endif
