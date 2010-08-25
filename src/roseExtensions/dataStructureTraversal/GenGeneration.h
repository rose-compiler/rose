#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "GenRepresentation.h"
#include "helpFunctions.h"
#include "ControlStructure.h" 


#define GC_CONDITIONAL_PRINT(variableName) (*dotout) << "#ifdef CGRAPHPP_USE_GC \n if ( GC_base(classReference->" + variableName + ") != NULL)\n #endif \n"
#define METHOD_PREFIX "save"

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

typedef std::vector <SgNode *, std::allocator <SgNode * > > SgNodePtrVector;

class
  GenGeneration
{
public:
  GenGeneration ();
  ~
  GenGeneration ();

  GenRepresentation* getPtrStaticGenRepresentation();
  void graphTypedefTranslationTable();
  void graphTypedefTranslationTable(map<SgTypedefDeclaration*, SgType*> mapSubset);

  void clear ();
  void printClass (SgNode * node, string nodelabel, string option);
  void printStruct (SgNode * node, string nodelabel, string option);
  void writeToFile (string filename);
  void writeStaticGraphToFile (string filename);
  void printCSVariables (list < SgNode * >classFields, string className,
                         SgClassDeclaration * sageClassDeclaration);
  void printUnionVariables (list < SgNode * >unionFields,
                            list < SgNode * >classFields,
                            list < ControlStructureContainer * >pragmaList,
                            string unionVariableName);
  void printClassNotPrinted (list < SgNode * >classNotPrinted, string name);
  list<SgNode * > queryClassNotPrinted (list < SgNode * >classFields,
                                        set < SgNode * >printedClasses,
                                        string variablePrefix = "");
  SgVariableDeclaration*
  queryFindUnionControlVariableDeclaration (SgNode * subTree,
					    string unionName);
  string generateCodeFindActiveUnionVariableTypeName (SgVariableDeclaration *
                                                      controlVariableDeclarataion);
  map<SgTypedefDeclaration*,SgType*> 
    buildTypedefTranslationTable(SgProject* project);

  ostringstream* dotout; 
  set <SgNode * > printedClasses;
  string classPointerName;
  string nameOfConstructedClass;
  string method_prefix;
private:
  map<SgTypedefDeclaration*, SgType*> typedefTranslationTable; 
  GenRepresentation* staticDataGraph;
};

