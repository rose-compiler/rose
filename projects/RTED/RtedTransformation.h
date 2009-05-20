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
  std::map<SgVarRefExp*, RTedArray*> create_array_access_call;
  // remember variables that were used to create an array. These cant be reused for array usage calls
  std::vector<SgVarRefExp*> createVariables;
  // ------------------------ string -----------------------------------
  // handle call to functioncall
  std::vector<RtedArguments*> function_call;


  // The following are vars that are needed for transformations
  // and retrieved through the visit function
  SgFunctionSymbol* roseCreateArray;
  SgFunctionSymbol* roseArrayAccess;
  SgFunctionSymbol* roseFunctionCall;
  SgFunctionSymbol* roseConvertIntToString;
  SgClassSymbol* runtimeClassSymbol;
  SgScopeStatement* rememberTopNode;
  SgStatement* mainLast;
  SgStatement* mainFirst;
  SgFunctionSymbol* roseRtedClose;
  bool insertMainBeforeLast;
  SgFunctionSymbol* roseCallStack;

  // FUNCTIONS ------------------------------------------------------------
  // Helper function
  SgStatement* getStatement(SgExpression* exp);
  // Transformation specific Helper Functions
  SgStatement* getSurroundingStatement(SgNode* n);
  // insert: RuntimeSystem* runtimeSystem = new RuntimeSystem();
  void insertRuntimeSystemClass();
  SgExpression* buildString(std::string name);




  // Traverse all nodes and check properties
  virtual void visit(SgNode* n);

  void insertMainCloseCall(SgStatement* main);
  void visit_checkIsMain(SgNode* n);
  void visit_isArraySgInitializedName(SgNode* n);
  void visit_isArraySgAssignOp(SgNode* n);
  void visit_isArrayPntrArrRefExp(SgNode* n);
  void visit_isArrayExprListExp(SgNode* n);

  void addPaddingToAllocatedMemory(SgStatement* stmt,  RTedArray* array);

  // Function that inserts call to array : runtimeSystem->callArray
  void insertArrayCreateCall(SgVarRefExp* n, RTedArray* value);
  void insertArrayCreateCall(SgInitializedName* initName,  RTedArray* value);
  void insertArrayCreateCall(SgStatement* stmt,SgInitializedName* initName,  RTedArray* value);

  void insertArrayAccessCall(SgVarRefExp* n, RTedArray* value);
  void insertArrayAccessCall(SgStatement* stmt,
			SgInitializedName* initName, RTedArray* array);

  std::pair<SgInitializedName*,SgVarRefExp*> getRightOfDot(SgDotExp* dot , std::string str, SgVarRefExp* varRef);
  std::pair<SgInitializedName*,SgVarRefExp*> getRightOfArrow(SgArrowExp* arrow , std::string str, SgVarRefExp* varRef);
  std::pair<SgInitializedName*,SgVarRefExp*> getPlusPlusOp(SgPlusPlusOp* plus ,std::string str, SgVarRefExp* varRef);
  std::pair<SgInitializedName*,SgVarRefExp*> getMinusMinusOp(SgMinusMinusOp* minus ,std::string str, SgVarRefExp* varRef);

  int getDimension(SgInitializedName* initName);
  int getDimension(SgInitializedName* initName,SgVarRefExp* varRef);
  SgVarRefExp* resolveToVarRefRight(SgExpression* expr);
  SgVarRefExp* resolveToVarRefLeft(SgExpression* expr);
  RtedSymbols* symbols;

  //  std::vector<RTedFunctionCall*> create_function_call;
  bool isVarRefInCreateArray(SgInitializedName* search);
  void insertFuncCall(RtedArguments* args);
  void insertStackCall(RtedArguments* args);
  void insertStackCall(RtedArguments* args, bool before);
  //void insertFunctionCall(RTedFunctionCall* funcCall, 
  //			  bool before);
  //void insertFuncCall(RtedArguments* args, bool before);
  void visit_isFunctionCall(SgNode* n);
  bool isInterestingFunctionCall(std::string name);
  int getDimensionForFuncCall(std::string name);

  std::string removeSpecialChar(std::string str);

 public:
  RtedTransformation() {
    //inputFiles=files;
    globalScope=NULL;
    roseCreateArray=NULL;
    roseArrayAccess=NULL;
    roseFunctionCall=NULL;
    roseConvertIntToString=NULL;
    roseCallStack=NULL;
    symbols = new RtedSymbols();
    mainFirst=NULL;
    mainLast=NULL;
    insertMainBeforeLast=false;
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


};

#endif
