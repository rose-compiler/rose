#include "a_nodes.h"

namespace Ada_ROSE_Translation 
   {
     class ASIS_Attribute : public AstAttribute
        {
       // This attribute is used to hold references from the SgUntypedNode IR to the ASIS data structures (using the Asis element ids).
          public:
            // Element_Struct* element;
            // persistantAttribute (Element_Struct* element) : element(element) {}
               int element_id;
               ASIS_Attribute (int element_id) : element_id(element_id) {}
        };

     void ada_to_ROSE_translation(List_Node_Struct *head_node, SgSourceFile* file);

     void processContext (Node_Union & nodeUnion);
     void processUnit    (Node_Union & nodeUnion);
     void processElement (Node_Union & nodeUnion);

     void processStatement   ( Statement_Struct     & statement,    int element_id);
     void processDefiningName( Defining_Name_Struct & definingName, int element_id);
     void processDeclaration ( Declaration_Struct   & declaration,  int element_id);
     void processExpression  ( Expression_Struct    & expression);
     void processDefinition  ( Definition_Struct    & definition);
     void processClause      ( Clause_Struct        & clause);
     void processAssociation ( Association_Struct   & association);

     void processExceptionHandler( Exception_Handler_Struct & exceptionHandler);

     void processPath        ( Path_Struct   & path);

     void processDefiningNameList          ( Defining_Name_List & Label_Names);
     void processPathList                  ( Path_List & pathList);
     void processAssociationList           ( Association_List & associationList);
     void processParameterSpecificationList( Parameter_Specification_List & parameterSpecificationList);
     void processStatementList             ( Statement_List & statementList);
     void processElementList               ( Element_List & elementList);
     void processRepresentationClauseList  ( Representation_Clause_List & representationClauseList);
     void processDeclarativeItemList       ( Declarative_Item_List & declarativeItemList);
     void processExceptionHandlerList      ( Exception_Handler_List & exceptionHandlerList);
     void processExpressionList            ( Expression_List & expressionList);
     void processDeclarationList           ( Declaration_List & declarationList);
     void processUnitList                  ( Unit_List & unitList);

     void processContentClauseList         ( Context_Clause_List & contentClauseList);
     void processPragmaElementList         ( Pragma_Element_List & pragmaElementList);

  // Supporting function
     void processElementIdList( Element_ID_Array_Struct & elementIdList);
     void processUnitIdList   ( Unit_ID_Array_Struct    & unitIdList);

  // Debugging support.
     std::string elementKindName           (Element_Kinds x);
     std::string enclosingKindName         (Enclosing_Kinds x);
     std::string statementKindName         (Statement_Kinds x);
     std::string definingNameKindName      (Defining_Name_Kinds x);
     std::string declarationKindName       (Declaration_Kinds x);
     std::string expressionKindName        (Expression_Kinds x);
     std::string definitionKindName        (Definition_Kinds x);
     std::string clauseKindName            (Clause_Kinds x);
     std::string associationKindName       (Association_Kinds x);
     std::string pathKindName              (Path_Kinds x);
     std::string operatorKindName          (Operator_Kinds x);
     std::string traitKindName             (Trait_Kinds x);
     std::string subprogramDefaultKindName (Subprogram_Default_Kinds x);
     std::string modeKindName              (Mode_Kinds x);
     std::string declarationOriginName     (Declaration_Origins x);
     std::string unitKindName              (Unit_Kinds x);
     std::string unitClassName             (Unit_Classes x);
     std::string unitOriginName            (Unit_Origins x);

  // Support for adding attributes (to the SgUntypedNodes).
     void setAsisAttribute ( SgUntypedNode* untypedNode, int element_id);
     int  getAsisAttribute ( SgUntypedNode* untypedNode);

  // This is the map from Ada node Id numbers to constructed SgUntypedNodes 
  // used to build the intermediate AST and from which we will define traversals 
  // to support the construction of the ROSE IR.
  // typedef std::map<int,SgUntypedNode*> MapType;
     typedef std::map<int,SgUntypedNode*>  ASIS_element_id_to_ROSE_MapType;
     typedef std::map<int,Element_Struct*> ASIS_element_id_to_ASIS_MapType;

  // Data Members
  // extern MapType untypedNodeMap;
     extern ASIS_element_id_to_ROSE_MapType untypedNodeMap;
     extern ASIS_element_id_to_ASIS_MapType asisMap;

     extern Unit_Struct*   globalUnit;
     extern SgUntypedFile* globalUntypedFile;

  // Debugging support
     void testTraversal();

   }


