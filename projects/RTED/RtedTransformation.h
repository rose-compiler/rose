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
  std::map<std::string,std::vector<std::string> > inputFiles;
  // the globalScope = SgProject
  SgGlobal* globalScope;
  // The array of callArray calls that need to be inserted
  std::map<SgInitializedName*, SgExpression*> create_array_call;

  // The following are vars that are needed for transformations
  // and retrieved through the visit function
  SgMemberFunctionSymbol* roseCreateArray;
  SgClassSymbol* runtimeClassSymbol;
  SgScopeStatement* rememberTopNode;

  // FUNCTIONS ------------------------------------------------------------
  // print information about input files
  void printInputFiles();
  std::string getFileName(std::string& lang, std::string& dir, std::string& file);
  // Transformation specific Helper Functions
  SgStatement* getSurroundingStatement(SgNode* n);

  // insert: RuntimeSystem* runtimeSystem = new RuntimeSystem();
  void insertRuntimeSystemClass();

  // Traverse all nodes and check properties
  virtual void visit(SgNode* n);

  // Function that inserts call to array : runtimeSystem->callArray
  void insertArrayCreateCall(SgInitializedName* name,SgExpression* value);

 public:
  RtedTransformation(std::map<std::string,std::vector<std::string> >& files) {
    inputFiles=files;
    globalScope=NULL;
    create_array_call.clear();
    roseCreateArray=NULL;
  };
  virtual ~RtedTransformation(){
    inputFiles.clear();

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
