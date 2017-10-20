#include "a_nodes.h"

namespace Ada_ROSE_Translation 
   {
     const int UNTYPED_FILE_ID = 0;
     const int MAX_NUMBER_OF_UNITS = 10;

     class ASIS_Attribute : public AstAttribute
        {
       // This attribute is used to hold references from the SgUntypedNode IR to the ASIS data structures (using the Asis element ids).
          public:
            // Element_Struct* element;
            // persistantAttribute (Element_Struct* element) : element(element) {}
               int element_id;
            // ASIS_Attribute (int element_id) : element_id(element_id) {}
               ASIS_Attribute (int element_id);

            // Need to define this virtual function to be more info in the generated nodes in the generated dot file.
            // virtual std::string toString();
            // std::string toString() { std::string s = "element_id = "; s += Rose::numberToString(element_id); return s; }
               std::string toString();
        };


  // Enum for different types of expressions.  These are used with the SgUntypedExpression IR nodes.
     enum ExpressionKind
        {
          e_unknown,
          e_error,
          e_identifier,
          e_characterLiteral,
          e_enumerationLiteral,
          e_integerLiteral,
          e_realLiteral,
          e_stringLiteral,
          e_selectedComponent,
       // e_operator,
          e_notAnOperator,
          e_andOperator,
          e_orOperator,
          e_xorOperator,
          e_equalOperator,
          e_notEqualOperator,
          e_lessThanOperator,
          e_lessThanOrEqualOperator,
          e_greaterThanOperator,
          e_greaterThanOrEqualOperator,
          e_plusOperator,
          e_minusOperator,
          e_concatenateOperator,
          e_unaryPlusOperator,
          e_unaryMinusOperator,
          e_multiplyOperator,
          e_divideOperator,
          e_modOperator,
          e_remOperator,
          e_exponentiateOperator,
          e_absOperator,
          e_notOperator,
          e_functionCall,
          e_typeConversion,
          e_qualifiedExpression,
          e_parameterAssociation,
          e_genericAssociation,
          e_last
        };

  // void ada_to_ROSE_translation(List_Node_Struct *head_node, SgSourceFile* file);
     void ada_to_ROSE_translation(Nodes_Struct & head_nodes, SgSourceFile* file);

  // void processContext (Node_Union & nodeUnion);
  // void processUnit    (Node_Union & nodeUnion);
  // void processElement (Node_Union & nodeUnion);
     void processContext (Context_Struct & context);
     void processUnit    (Unit_Structs_Ptr units);
     void processElement (Element_Structs_Ptr elements);

     void processStatement   ( Statement_Struct     & statement,    int element_id);
     void processDefiningName( Defining_Name_Struct & definingName, int element_id);
     void processDeclaration ( Declaration_Struct   & declaration,  int element_id);
     void processExpression  ( Expression_Struct    & expression,   int element_id);
     void processDefinition  ( Definition_Struct    & definition,   int element_id);
     void processClause      ( Clause_Struct        & clause);
     void processAssociation ( Association_Struct   & association,  int element_id);

     void processExceptionHandler( Exception_Handler_Struct & exceptionHandler, int element_id);

     void processPath        ( Path_Struct   & path,  int element_id);

  // DQ (10/12/2017): Reorganized Definition_Struct by Charles,
     void processTypeDefinition              ( Type_Definition_Struct & x,  int element_id, bool & buildDefaultUntypedNode);
     void processSubtypeIndication           ( Subtype_Indication_Struct & x,  int element_id);
     void processConstraint                  ( Constraint_Struct & x,  int element_id);
     void processComponentDefinition         ( Component_Definition_Struct & x,  int element_id);
     void processDiscreteSubtypeDefinition   ( Discrete_Subtype_Definition_Struct & x,  int element_id);
     void processDiscreteRange               ( Discrete_Range_Struct & x,  int element_id);
     void processUnknownDiscriminantPart     ( Unknown_Discriminant_Part_Struct & x,  int element_id);
     void processKnownDiscriminantPart       ( Known_Discriminant_Part_Struct & x,  int element_id);
     void processRecordDefinition            ( Record_Definition_Struct & x,  int element_id);
     void processNullRecordDefinition        ( Null_Record_Definition_Struct & x,  int element_id);
     void processNullComponent               ( Null_Component_Struct & x,  int element_id);
     void processVariantPart                 ( Variant_Part_Struct & x,  int element_id);
     void processVariant                     ( Variant_Struct & x,  int element_id);
     void processOthersChoice                ( Others_Choice_Struct & x,  int element_id);
     void processAccessDefinition            ( Access_Definition_Struct & x,  int element_id);
     void processPrivateTypeDefinition       ( Private_Type_Definition_Struct & x,  int element_id);
     void processTaggedPrivateTypeDefinition ( Tagged_Private_Type_Definition_Struct & x,  int element_id);
     void processPrivateExtensionDefinition  ( Private_Extension_Definition_Struct & x,  int element_id);
     void processTaskDefinition              ( Task_Definition_Struct & x,  int element_id);
     void processProtectedDefinition         ( Protected_Definition_Struct & x,  int element_id);
     void processFormalTypeDefinition        ( Formal_Type_Definition_Struct & x,  int element_id);
     void processAspectSpecification         ( Aspect_Specification_Struct & x,  int element_id);

  // DQ (10/12/2017): Reorganized Definition_Struct by Charles,
  // void processAccessType                  ( Access_Type_Struct & x,  int element_id);
     void processAccessType                  ( Access_Type_Struct & x);

     void processDefiningNameList          ( Defining_Name_List & Label_Names);
     void processPathList                  ( Path_List & pathList);
     void processAssociationList           ( Association_List & associationList);
     void processParameterSpecificationList( Parameter_Specification_List & parameterSpecificationList);
     void processStatementList             ( Statement_List & statementList);
     void processElementList               ( Element_ID_List & elementList);
     void processRepresentationClauseList  ( Representation_Clause_List & representationClauseList);
     void processDeclarativeItemList       ( Declarative_Item_List & declarativeItemList);
     void processExceptionHandlerList      ( Exception_Handler_List & exceptionHandlerList);
     void processExpressionList            ( Expression_List & expressionList);
     void processDeclarationList           ( Declaration_List & declarationList);
     void processUnitList                  ( Unit_List & unitList);

     void processRecordComponentList       ( Record_Component_List & recordComponentList);

  // DQ (10/12/2017): Fails with update from Charles.
  // void processDeclarativeItemIdList     ( Declarative_Item_ID_List & declarativeItemIdList);

  // DQ (10/12/2017): Required for new update from Charles.
     void processDiscreteRangeList           ( Discrete_Range_List & discreteRangesList);
     void processDiscriminantAssociationList ( Discriminant_Association_List & discriminantAssociationList);


     void processContentClauseList         ( Context_Clause_List & contentClauseList);
  // void processPragmaElementList         ( Pragma_Element_List & pragmaElementList);
     void processPragmaElementList         ( Pragma_Element_ID_List & pragmaElementList);


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

  // DQ (10/12/2017): Fails with update from Charles.
  // std::string traitKindName             (Trait_Kinds x);

  // DQ (10/12/2017): Required for new update from Charles.
     std::string typeKindName          (Type_Kinds x);

     std::string subprogramDefaultKindName (Subprogram_Default_Kinds x);
     std::string modeKindName              (Mode_Kinds x);
     std::string declarationOriginName     (Declaration_Origins x);
     std::string attributeKindName         (Attribute_Kinds x);

  // DQ (10/12/2017): Required for new update from Charles.
     std::string rootTypeKindName             (Root_Type_Kinds x);
     std::string interfaceKindName            (Interface_Kinds x);
     std::string accessDefinitionKindName     (Access_Definition_Kinds x);
     std::string formalTypeKindName           (Formal_Type_Kinds x);
     std::string discreteRangeKindName        (Discrete_Range_Kinds x);
     std::string constraintKindName           (Constraint_Kinds x);
     std::string representationClauseKindName (Representation_Clause_Kinds x);
     std::string accessTypeKindName           (Access_Type_Kinds x);

     std::string unitKindName              (Unit_Kinds x);
     std::string unitClassName             (Unit_Classes x);
     std::string unitOriginName            (Unit_Origins x);


  // Support functions to identify children and assembel them into the SgUntypedStatementList IR node.
     void populateChildrenFromDeclaration(SgUntypedStatementList* statementList, Declaration_Struct & declaration);
     void populateChildrenFromDefinition (SgUntypedStatementList* statementList, Definition_Struct  & definition);

  // Abstract processing of SgUntypedNode to include into maps, and attach attribute.
     void processUntypedNode (SgUntypedNode* node, int element_id);

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

  // DQ (10/15/2017): Remove global unit, since a file's translation unit can have several (and they are traversed within ASIS).
  // extern Unit_Struct*   globalUnit;
     extern SgUntypedFile* globalUntypedFile;

  // Debugging support
     void testTraversal();

   }


