#ifndef RTEDTRANS_H
#define RTEDTRANS_H

#include "RtedSymbols.h"


/* -----------------------------------------------------------
 * tps : 6March 2009: This class adds transformations
 * so that runtime errors are caught at runtime before they happen
 * -----------------------------------------------------------*/
class RtedTransformation : public AstSimpleProcessing {
 private:
  // VARIABLES ------------------------------------------------------------
  // list of input files (all RTED files)
  //std::vector<std::string> inputFiles;
  // the globalScope = SgProject
  SgGlobal* globalScope;
  // The array of callArray calls that need to be inserted
  std::map<SgVarRefExp*, RTedArray*> create_array_define_varRef_multiArray;
  std::map<SgInitializedName*, RTedArray*> create_array_define_varRef_multiArray_stack;
  std::map<SgVarRefExp*, RTedArray*> create_array_access_call;
  // remember variables that were used to create an array. These cant be reused for array usage calls
  std::vector<SgVarRefExp*> createVariables;


  // The following are vars that are needed for transformations
  // and retrieved through the visit function
  SgMemberFunctionSymbol* roseCreateArray;
  SgMemberFunctionSymbol* roseArrayAccess;
  SgClassSymbol* runtimeClassSymbol;
  SgScopeStatement* rememberTopNode;

  // FUNCTIONS ------------------------------------------------------------
  // Helper function
  SgStatement* getStatement(SgExpression* exp);
  // Transformation specific Helper Functions
  SgStatement* getSurroundingStatement(SgNode* n);
  // insert: RuntimeSystem* runtimeSystem = new RuntimeSystem();
  void insertRuntimeSystemClass();

  // Traverse all nodes and check properties
  virtual void visit(SgNode* n);

  // Function that inserts call to array : runtimeSystem->callArray
  void insertArrayCreateCall(SgVarRefExp* n, RTedArray* value);
  void insertArrayCreateCall(SgInitializedName* initName,  RTedArray* value);
  void insertArrayCreateCall(SgStatement* stmt,SgInitializedName* initName,  RTedArray* value);

  void insertArrayAccessCall(SgVarRefExp* n, RTedArray* value);
  void insertArrayAccessCall(SgStatement* stmt,
			SgInitializedName* initName, RTedArray* array);

  SgInitializedName* getRightOfDot(SgDotExp* dot , std::string str);
  SgInitializedName* getRightOfArrow(SgArrowExp* arrow , std::string str);
  SgInitializedName* getPlusPlusOp(SgPlusPlusOp* plus ,std::string str);
  SgInitializedName* getMinusMinusOp(SgMinusMinusOp* minus ,std::string str);

  int getDimension(SgInitializedName* initName);
  RtedSymbols* symbols;

 public:
  RtedTransformation() {
    //inputFiles=files;
    globalScope=NULL;
    roseCreateArray=NULL;
    roseArrayAccess=NULL;
    symbols = new RtedSymbols();
  };
  virtual ~RtedTransformation(){
  //  inputFiles.clear();

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
