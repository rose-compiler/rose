//#include "rose.h"
#include "helpFunctions.h"
#include "GenRepresentation.h"
#include "ControlStructure.h" 
#include "AttributeType.h"

#define GENGEN_DEBUG TRUE

#define GC_CONDITIONAL_PRINT(variableName,AT) (*attribute) ( "\n#if CGRAPHPP_USE_GC \n if (GCpointerCheck("+variableName+")==1)\n #else \nif ((" + variableName + ")!=NULL)\n#endif\n", AT);
#define METHOD_PREFIX "save"
//typedef MidLevelCollectionTypedefs::PlacementPositionEnum PlacementPositionEnum
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
  enum TypeDef 
    {
       UnknownListElement = 0,
       an_class_type  = 1,
       an_struct_type = 2,
       an_union_type  = 3,
       an_class_type_ptr   = 4,
       an_struct_type_ptr  = 5,  
       an_anonymous_class  = 6,
       an_anonymous_struct = 7,
       an_anonymous_union  = 8,
       END_OF_LIST 
    }; 


  void clear ();
  void printClass (SgNode * node, std::string nodelabel, std::string option);
  void printStruct (SgNode * node, std::string nodelabel, std::string option);
  void writeToFile (std::string filename);
  void writeStaticGraphToFile (std::string filename);
  void printGlobalScope(SgGlobal* sageGlobal, Rose_STL_Container<SgNode*> globalDeclarationLst);
  void printType(SgType* sageType);

  AttributeType* printCSVariables (AttributeType* inheritedAttribute,  SgClassDeclaration*, Rose_STL_Container< SgNode * >classFields, std::string className, std::string variablePrefix = "classReference->");
  AttributeType* printClassAsVariable (AttributeType* inheritedAttribute,  SgClassDeclaration* sageClassDeclaration,
  Rose_STL_Container<SgNode*> classFields, const std::string unionVariableName,  const std::string variablePrefix = "classReference->");
  void printClassNotPrinted (Rose_STL_Container<SgNode*> classNotPrinted, std::string name, std::string variablePrefix = "classReference->");
  Rose_STL_Container<SgNode*> queryClassNotPrinted (AttributeType* attribute, Rose_STL_Container<SgNode*> classFields,
			std::set < SgNode * >printedClasses,
			TypeDef classDeclType = UnknownListElement, std::string variablePrefix = "classReference->" );
  SgVariableDeclaration*
  queryFindUnionControlVariableDeclaration (SgNode * subTree, std::string unionName);
  std::string generateCodeFindActiveUnionVariableTypeName (SgVariableDeclaration* controlVariableDeclarataion);

  std::ostringstream* dotout;
  std::ostringstream* header;
  std::set <SgNode * > printedClasses;
  std::string classPointerName,nameOfConstructedClass,method_prefix;
private:
  AttributeType* classTypePrint(AttributeType*, SgClassDeclaration*, SgVariableDeclaration*,
                               std::string, GenRepresentation::TypeOfPrint);
  AttributeType* classTypePrintAsVariable(AttributeType*, SgClassDeclaration*, SgVariableDeclaration*,
                              Rose_STL_Container<SgNode*>, std::string, std::string, std::string, GenRepresentation::TypeOfPrint);	
  std::set<void*> traversedTypes;
  GenRepresentation* staticDataGraph;
};

