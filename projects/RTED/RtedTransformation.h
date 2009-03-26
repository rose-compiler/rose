#ifndef RTEDTRANS_H
#define RTEDTRANS_H

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
  std::map<SgInitializedName*, SgExpression*> create_array_call;
  std::map<SgVarRefExp*, SgExpression*> create_array_access_call;

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
  void insertArrayCreateCall(SgInitializedName* name,SgExpression* value);
  void insertArrayAccessCall(SgVarRefExp* n, SgExpression* value);

 public:
  RtedTransformation() {
    //inputFiles=files;
    globalScope=NULL;
    create_array_call.clear();
    roseCreateArray=NULL;
    roseArrayAccess=NULL;
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
