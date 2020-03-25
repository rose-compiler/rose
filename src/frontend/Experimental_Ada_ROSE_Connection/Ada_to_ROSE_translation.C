
// #include "rose.h"

// DQ (11/13/2017): This is a violation, sage3basic.h must be the first file included.
// #include "rose_config.h"

#include "sage3basic.h"

#include "rose_config.h"

#include "Ada_to_ROSE_translation.h"
//~ #include "untypedBuilder.h"

using namespace std;

#if 0
namespace Ada_ROSE_Translation
   {
  // Data Members
     MapType untypedNodeMap;


   }
#else
// Ada_ROSE_Translation::MapType Ada_ROSE_Translation::untypedNodeMap;
Ada_ROSE_Translation::ASIS_element_id_to_ROSE_MapType Ada_ROSE_Translation::untypedNodeMap;
Ada_ROSE_Translation::ASIS_element_id_to_ASIS_MapType Ada_ROSE_Translation::asisMap;

// DQ (10/15/2017): Remove global unit, since a file's translation unit can have several (and they are traversed within ASIS).
// Unit_Struct*   Ada_ROSE_Translation::globalUnit        = NULL;
SgUntypedFile* Ada_ROSE_Translation::globalUntypedFile = NULL;
#endif

namespace Ada_ROSE_Translation
{
  void secondConversion(Nodes_Struct& head_nodes, SgSourceFile* file);
}

// Attribute constructor.
Ada_ROSE_Translation::ASIS_Attribute::ASIS_Attribute (int element_id)
   : element_id(element_id)
   {
   }

// Attribute toString function (used by Dot file generator).
string
Ada_ROSE_Translation::ASIS_Attribute::toString()
   {
     string s = "element_id = ";
     s += Rose::StringUtility::numberToString(element_id);
     return s;
   }


void
Ada_ROSE_Translation::setAsisAttribute ( SgUntypedNode* untypedNode, int element_id)
   {
  // Add attribute to store reference to ASIS derived data.

  // Build an attribute (on the heap)
     AstAttribute* newAttribute = new ASIS_Attribute(element_id);
     ROSE_ASSERT(newAttribute != NULL);

  // Add it to the AST (so it can be found later in another pass over the AST)
     untypedNode->addNewAttribute("ASIS",newAttribute);
   }


int
Ada_ROSE_Translation::getAsisAttribute ( SgUntypedNode* untypedNode)
   {
  // Retrive the attribute from the untyped IR node.
     ASIS_Attribute* attribute = dynamic_cast<ASIS_Attribute*>(untypedNode->getAttribute("ASIS"));
     ROSE_ASSERT(attribute != NULL);

  // Add it to the AST (so it can be found later in another pass over the AST)
     int element_id = attribute->element_id;

     printf ("In getAsisAttribute(): Found element_id = %d \n",element_id);

     return element_id;
   }


void
Ada_ROSE_Translation::processUntypedNode (SgUntypedNode* untypedNode, int element_id)
   {
     printf ("In processUntypedNode: untypedNode = %p = %s element_id = %d \n",untypedNode,untypedNode->class_name().c_str(),element_id);

  // Put into map using Ada Node ID's as the keys.
     ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
     untypedNodeMap[element_id] = untypedNode;
     ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

     printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

  // Build the attribute (element id onto the SgUntypedNode).
     setAsisAttribute (untypedNode,element_id);
   }


// void Ada_ROSE_Translation::ada_to_ROSE_translation(List_Node_Struct *head_node, SgSourceFile* file)
void
Ada_ROSE_Translation::ada_to_ROSE_translation(Nodes_Struct & head_nodes, SgSourceFile* file)
   {
#if 0
     printf ("Start of initial traversal of Ada IR node data structure \n");
#endif

  // Traverse the C data structure representing the IR nodes in the Ada AST (building the asisMap).
  // struct List_Node_Struct *current_node = NULL;
     Element_Struct_List_Struct *current_element = NULL;

     current_element = head_nodes.Elements;

#if 0
     printf ("current_element = %p \n",current_element);
#endif

     while (current_element != NULL)
        {
#if 0
          printf ("Initial traversal: current_element: Next_Count = %d \n",current_element->Next_Count);
#endif
       // Build the asisMap of Element ids to Element_Struct pointers.
       // if (current_node->Node.Node_Kind == An_Element_Node)
       // if (current_element->Node.Node_Kind == An_Element_Node)
             {
            // Element_Struct & element    = current_node->Node.The_Union.Element;
               Element_Struct & element    = current_element->Element;
               Element_ID       element_id = element.ID;
#if 0
               printf ("Initialize the asisMap with each Element_Struct: element_id = %d \n",element_id);
#endif
               ROSE_ASSERT(element_id > MAX_NUMBER_OF_UNITS);
#if 1
            // This code treats duplicate entries as a warning and skips the redundant entry.
               if (asisMap.find(element_id) == asisMap.end())
                  {
                    std::cerr << "***** adding element " << element_id << std::endl;
                    asisMap[element_id] = &element;
                  }
                 else
                  {
                    printf ("ERROR: element_id = %d already processed (skipping additional instance) \n",element_id);
                  }
               ROSE_ASSERT(asisMap.find(element_id) != asisMap.end());
#else
            // This is the better code that treats the case of duplicate entries as an error.
               ROSE_ASSERT(asisMap.find(element_id) == asisMap.end());
               asisMap[element_id] = &element;
               ROSE_ASSERT(asisMap.find(element_id) != asisMap.end());
#endif
             }
#if 0
          printf ("current_element->Next = %p \n",current_element->Next);
#endif
          current_element = current_element->Next;
        }

     Unit_Struct_List_Struct *current_unit = NULL;

     current_unit = head_nodes.Units;

#if 0
     printf ("current_unit = %p \n",current_unit);
#endif
     std::cerr << "***** adding units (element) MAX: " << MAX_NUMBER_OF_UNITS << std::endl;

     while (current_unit != NULL)
        {
#if 0
          printf ("Initial traversal: current_unit: Next_Count = %d \n",current_unit->Next_Count);
#endif
       // if (current_node->Node.Node_Kind == A_Unit_Node)
             {
               Unit_Struct & unit = current_unit->Unit;
            // globalUnit = &unit;
#if 0
               printf ("Initialize Unit: filename = %s \n",unit.Text_Name);
               printf ("Initialize Unit: Declaration_ID = %d \n",unit.Unit_Declaration);
#endif
            // This code treats duplicate entries as a warning and skips the redundant entry.
               int unit_id = unit.ID;

            // We are using the unit id for the key into the element list (messy).
               ROSE_ASSERT(unit_id > 0);
               ROSE_ASSERT(unit_id <= MAX_NUMBER_OF_UNITS);

               if (asisMap.find(unit_id) == asisMap.end())
                  {
                    std::cerr << "***** adding unit (element) " << unit_id << std::endl;
                    asisMap[unit_id] = (Element_Struct*) (&unit);
                  }
                 else
                  {
                    printf ("ERROR: element_id = %d already processed (skipping additional instance) \n",unit_id);
                  }

               ROSE_ASSERT(asisMap.find(unit_id) != asisMap.end());
             }
#if 0
          printf ("current_unit->Next = %p \n",current_unit->Next);
#endif
          current_unit = current_unit->Next;
        }

#if 1
     printf ("End of initial traversal of Ada IR node data structure: asisMap.size() = %zu \n",asisMap.size());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     printf ("Start of translation traversal of Ada IR node data structure \n");

     current_element = head_nodes.Elements;
     while (current_element != NULL)
        {
#if 0
          printf ("current_element: Next_Count = %d \n",current_element->Next_Count);
#endif
          processElement(current_element);
#if 0
          printf ("current_element->Next = %p \n",current_element->Next);
#endif
          current_element = current_element->Next;
        }

     current_unit = head_nodes.Units;
     while (current_unit != NULL)
        {
#if 0
          printf ("current_unit: Next_Count = %d \n",current_unit->Next_Count);
#endif
          processUnit(current_unit);
#if 0
          printf ("current_unit->Next = %p \n",current_unit->Next);
#endif
          current_unit = current_unit->Next;
        }

     processContext(head_nodes.Context);

  // Check that these have been set.
  // ROSE_ASSERT(globalUnit != NULL);
     ROSE_ASSERT(globalUntypedFile != NULL);

#if 1
     printf ("End of translation traversal of Ada IR node data structure \n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 1
     printf ("Connect up the SgUntypedNodes for form an AST \n");
#endif

  // Add the SgUntypedFile to the untypedNodeMap (use zero as element_id).
  // Put into map using Ada Node ID's as the keys.
     int file_id = 0;
     ROSE_ASSERT(untypedNodeMap.find(file_id) == untypedNodeMap.end());
     untypedNodeMap[file_id] = globalUntypedFile;
     ROSE_ASSERT(untypedNodeMap.find(file_id) != untypedNodeMap.end());

#if 0
     printf ("Need to divide up map into seperate maps for units and elements \n");
     ROSE_ASSERT(false);
#endif

  // Connect up the SgUntypedNodes for form an AST.
     for (ASIS_element_id_to_ROSE_MapType::iterator i = untypedNodeMap.begin(); i != untypedNodeMap.end(); i++)
        {
       // Iterate over the SgUntypedNodes
          SgUntypedNode* untypedNode = i->second;
#if 0
          printf ("untypedNode = %p = %s \n",untypedNode,untypedNode->class_name().c_str());
#endif
       // We need to connect children to parents from the parent perspective.

       // untypedNode = 0x7facf6247010 = SgUntypedInitializedName
       // untypedNode = 0x7fc9f5aa3010 = SgUntypedFunctionDeclaration
       // untypedNode = 0x7facf6272010 = SgUntypedNamedStatement

          if (untypedNode == NULL) continue;

          switch (untypedNode->variantT())
             {
               case V_SgUntypedFile:
                  {
                    printf ("Found a SgUntypedFile \n");
                    SgUntypedFile* file = isSgUntypedFile(untypedNode);
                    ROSE_ASSERT(file != NULL);
#if 1
                 // File the associated children.
                 // The children are now listed in the Unit_Struct

                 // Order of appearance in compilation unit:
                 //    1) Compilation_Pragmas
                 //    2) Context_Clause_Elements
                 //    3) Unit_Declaration
#if 0
                    if (globalUnit->Compilation_Pragmas.Length > 0)
                       {
                         printf ("Skipping processing of Compilation_Pragmas \n");
                       }

                    if (globalUnit->Context_Clause_Elements.Length > 0)
                       {
                         printf ("Skipping processing of Context_Clause_Elements \n");
                       }

                    int declaration_id = globalUnit->Unit_Declaration;

                    printf ("declaration_id = %d \n",declaration_id);
#endif
#if 0
                    SgUntypedNode* untypedNode = untypedNodeMap[declaration_id];
                    ROSE_ASSERT(untypedNode != NULL);

                    printf ("untypedNode = %p = %s \n",untypedNode,untypedNode->class_name().c_str());
#endif
                    SgUntypedGlobalScope* globalScope = file->get_scope();
                    ROSE_ASSERT(globalScope != NULL);
                    ROSE_ASSERT(globalScope->get_statement_list() != NULL);
                    SgUntypedStatementPtrList & untypedStatementPtrList = globalScope->get_statement_list()->get_stmt_list();
                 // untypedStatementPtrList.insert(untypedStatementPtrList.end(),functionDeclaration);
#if 1
                    printf ("untypedStatementPtrList.size() = %zu \n",untypedStatementPtrList.size());
#endif

                 // Find the attribute associated with the SgUntypedGlobalScope.
                    ASIS_Attribute* attribute = dynamic_cast<ASIS_Attribute*>(untypedNode->getAttribute("ASIS"));
                    if (attribute != NULL)
                       {
                         int globalScopeId = getAsisAttribute(globalScope);

                         printf ("globalScopeId = %d \n",globalScopeId);
                       }
#if 0
                 // Find the ASIS IR nodes that coresponds to our SgUntypedFile node
                    ROSE_ASSERT(globalUnit != NULL);

                 // Declaration_ID           bodyBlockStatement    = globalUnit->Body_Block_Statement;
                 // printf ("      bodyBlockStatement = %d \n",bodyBlockStatement);

                 // Unit_List         Corresponding_Children;

                    printf ("globalUnit->Corresponding_Children.Length = %d \n",globalUnit->Corresponding_Children.Length);
                    for (int i = 0; i < globalUnit->Corresponding_Children.Length; i++)
                       {
                         printf ("      child = %d \n",i);
                       }
#endif
                 // Note that zero is reserved for the SgUntypedFile IR node in out map.
                    for (int i = 1; i < MAX_NUMBER_OF_UNITS; i++)
                       {
                         printf ("Processing SgUntypedFile: testing unit_id = %d \n",i);
                         if (untypedNodeMap.find(i) != untypedNodeMap.end())
                            {
                              SgUntypedNode* untypedNode = untypedNodeMap[i];
                              ROSE_ASSERT(untypedNode != NULL);

                              printf ("Processing SgUntypedFile: identify units: untypedNode = %p = %s \n",untypedNode,untypedNode->class_name().c_str());

                              SgUntypedUnitDeclaration* untypedUnitDeclaration = isSgUntypedUnitDeclaration(untypedNode);
                              ROSE_ASSERT(untypedUnitDeclaration != NULL);

                              printf ("untypedUnitDeclaration->get_name() = %s \n",untypedUnitDeclaration->get_name().c_str());

                              untypedStatementPtrList.push_back(untypedUnitDeclaration);

                              untypedUnitDeclaration->set_parent(globalScope);
                            }

                       }

#if 0
                    printf ("case of SgUntypedFile not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    SgUntypedFunctionDeclaration* functionDeclaration = isSgUntypedFunctionDeclaration(untypedNode);
                    if (functionDeclaration != NULL)
                       {
#if 0
                      // Add this function declaration as a declaration in global scope.
                         SgUntypedGlobalScope* globalScope = file->get_scope();
                         ROSE_ASSERT(globalScope != NULL);
                         ROSE_ASSERT(globalScope->get_statement_list() != NULL);
                         SgUntypedStatementList* untypedStatementList = globalScope->get_statement_list();
                         ROSE_ASSERT(untypedStatementList != NULL);
                      // ROSE_ASSERT(globalScope->get_statement_list()->get_stmt_list() != NULL);
                      // globalScope->get_statement_list()->get_stmt_list().insert(functionDeclaration);
                         SgUntypedStatementPtrList & untypedStatementPtrList = globalScope->get_statement_list()->get_stmt_list();
#endif
                         untypedStatementPtrList.insert(untypedStatementPtrList.end(),functionDeclaration);
#if 1
                         printf ("untypedStatementPtrList.size() = %zu \n",untypedStatementPtrList.size());
#endif
                       }
#else
                 // OLD CODE (work around)
                 // Find the function declarations more directly, since children are not availalbe.
                    for (ASIS_element_id_to_ROSE_MapType::iterator i = untypedNodeMap.begin(); i != untypedNodeMap.end(); i++)
                       {
                      // Iterate over the SgUntypedNodes
                         SgUntypedNode* untypedNode = i->second;
                         SgUntypedFunctionDeclaration* functionDeclaration = isSgUntypedFunctionDeclaration(untypedNode);
                         if (functionDeclaration != NULL)
                            {
                           // Add this function declaration as a declaration in global scope.
                              SgUntypedGlobalScope* globalScope = file->get_scope();
                              ROSE_ASSERT(globalScope != NULL);
                              ROSE_ASSERT(globalScope->get_statement_list() != NULL);
                              SgUntypedStatementList* untypedStatementList = globalScope->get_statement_list();
                              ROSE_ASSERT(untypedStatementList != NULL);
                           // ROSE_ASSERT(globalScope->get_statement_list()->get_stmt_list() != NULL);
                           // globalScope->get_statement_list()->get_stmt_list().insert(functionDeclaration);
                              SgUntypedStatementPtrList & untypedStatementPtrList = globalScope->get_statement_list()->get_stmt_list();
                              untypedStatementPtrList.insert(untypedStatementPtrList.end(),functionDeclaration);
#if 0
                              printf ("untypedStatementPtrList.size() = %zu \n",untypedStatementPtrList.size());
#endif
                            }
                       }
#endif

#if 0
                    printf ("case of SgUntypedFile not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

               case V_SgUntypedInitializedName:
                  {
                 // This is a child node and we only visit parents to connect up children, so nothing to do here.

                    printf ("Found a SgUntypedInitializedName \n");
#if 0
                    printf ("case of SgUntypedInitializedName not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

               case V_SgUntypedFunctionDeclaration:
                  {
                 // This is the parent of any statements in the body of the function.
                    printf ("Found a SgUntypedFunctionDeclaration \n");

                    SgUntypedFunctionDeclaration* untypedFunctionDeclaration = isSgUntypedFunctionDeclaration(untypedNode);
                    ROSE_ASSERT(untypedFunctionDeclaration != NULL);

                    SgUntypedFunctionScope* untypedFunctionScope = untypedFunctionDeclaration->get_scope();

                 // If the function scope is not available, then this is a simple function prototype.
                 // ROSE_ASSERT(untypedFunctionScope != NULL);
                    if (untypedFunctionScope != NULL)
                       {
                         SgUntypedStatementList* untypedStatementList = untypedFunctionScope->get_statement_list();
                         ROSE_ASSERT(untypedStatementList != NULL);

                      // We don't need to look up the SgUntypedFunctionDeclaration.
                         int functionDeclaration_id = getAsisAttribute(untypedNode);

                         printf ("functionDeclaration_id = %d \n",functionDeclaration_id);
                      // xxx = untypedNodeMap[functionDeclaration_id];
                         ROSE_ASSERT(untypedNode == untypedNodeMap[functionDeclaration_id]);

                         Element_Struct* element_struct = asisMap[functionDeclaration_id];
                         ROSE_ASSERT(element_struct != NULL);

                         Element_Union & element_union = element_struct->The_Union;

                      // Declaration_Struct & declaration = element_union.Declaration;
                      // Declaration_Struct & declaration = element_struct->Declaration;
                         Declaration_Struct & declaration = element_union.Declaration;

                      // Now get the children of the Element_Struct.
                      // Statement_List & bodyStatementList = element_struct->Body_Statements;
                         Statement_List & bodyStatementList = declaration.Body_Statements;

                         for (int i = 0; i < bodyStatementList.Length; i++)
                            {
                              int statement_id = bodyStatementList.IDs[i];
                              printf ("statement_id = %d \n",statement_id);

                              SgUntypedNode* untypedNodeInBody = untypedNodeMap[statement_id];
                              ROSE_ASSERT(untypedNodeInBody != NULL);

                              printf ("untypedNodeInBody = %p = %s \n",untypedNodeInBody,untypedNodeInBody->class_name().c_str());

                              SgUntypedStatement* untypedStatement = isSgUntypedStatement(untypedNodeInBody);
                              ROSE_ASSERT(untypedStatement != NULL);

                              SgUntypedStatementPtrList & stmtList = untypedStatementList->get_stmt_list();

                           // untypedStatementList->get_stmt_list().insert(untypedStatementList->get_stmt_list().end(),untypedStatement);
                              stmtList.insert(stmtList.end(),untypedStatement);

                              printf ("untypedStatementList->get_stmt_list().size() = %zu \n",untypedStatementList->get_stmt_list().size());
                            }
                       }

#if 0
                    printf ("case of SgUntypedFunctionDeclaration not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

               case V_SgUntypedName:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedName \n");
#if 0
                    printf ("case of SgUntypedName not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedStructureDeclaration:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedStructureDeclaration \n");

                    printf ("ERROR: The data members are not yet in the AST, should be fixed in the upstream Ada support shortly \n");
#if 0
                    printf ("case of SgUntypedStructureDeclaration not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedVariableDeclaration:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedVariableDeclaration \n");
#if 0
                    printf ("case of SgUntypedVariableDeclaration not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

               case V_SgUntypedPackageDeclaration:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedPackageDeclaration \n");


                    SgUntypedPackageDeclaration* untypedPackageDeclaration = isSgUntypedPackageDeclaration(untypedNode);
                    ROSE_ASSERT(untypedPackageDeclaration != NULL);

                    SgUntypedScope* untypedScope = untypedPackageDeclaration->get_scope();

                    SgUntypedStatementList* untypedStatementList = untypedScope->get_statement_list();
                    ROSE_ASSERT(untypedStatementList != NULL);

                 // Get the reference to the ASIS data strucuture.
                    int declaration_id = getAsisAttribute(untypedNode);

                    printf ("declaration_id = %d \n",declaration_id);

                    ROSE_ASSERT(asisMap.find(declaration_id) != asisMap.end());

                    Element_Struct* element_struct = asisMap[declaration_id];
                    ROSE_ASSERT(element_struct != NULL);

                    Element_Union & element_union = element_struct->The_Union;

                    Declaration_Struct & declaration = element_union.Declaration;

                 // Now get the children of the Element_Struct.
                    Declarative_Item_List & visiblePartDeclarativeItems = declaration.Visible_Part_Declarative_Items;

                    for (int i = 0; i < visiblePartDeclarativeItems.Length; i++)
                       {
                         int statement_id = visiblePartDeclarativeItems.IDs[i];
                         printf ("i = %d statement_id = %d \n",i,statement_id);
#if 1
                         if (untypedNodeMap.find(statement_id) != untypedNodeMap.end())
                         {
                         SgUntypedNode* visiblePart_untypedNode = untypedNodeMap[statement_id];
                         ROSE_ASSERT(visiblePart_untypedNode != NULL);

                         printf ("visiblePart_untypedNode = %p = %s \n",visiblePart_untypedNode,visiblePart_untypedNode->class_name().c_str());

                         SgUntypedStatement* untypedStatement = isSgUntypedStatement(visiblePart_untypedNode);
                         ROSE_ASSERT(untypedStatement != NULL);

                         SgUntypedStatementPtrList & stmtList = untypedStatementList->get_stmt_list();

                      // untypedStatementList->get_stmt_list().insert(untypedStatementList->get_stmt_list().end(),untypedStatement);
                         stmtList.insert(stmtList.end(),untypedStatement);
                        }
#endif
                         printf ("untypedStatementList->get_stmt_list().size() = %zu \n",untypedStatementList->get_stmt_list().size());
                       }

                 // Now get the children of the Element_Struct.
                    Declarative_Item_List & privatePartDeclarativeItems = declaration.Private_Part_Declarative_Items;

                    for (int i = 0; i < privatePartDeclarativeItems.Length; i++)
                       {
                         int statement_id = privatePartDeclarativeItems.IDs[i];
                         printf ("i = %d statement_id = %d \n",i,statement_id);

                         ROSE_ASSERT(untypedNodeMap.find(statement_id) != untypedNodeMap.end());

                         SgUntypedNode* private_untypedNode = untypedNodeMap[statement_id];
                         ROSE_ASSERT(private_untypedNode != NULL);

                         printf ("private_untypedNode = %p = %s \n",private_untypedNode,private_untypedNode->class_name().c_str());

                         SgUntypedStatement* untypedStatement = isSgUntypedStatement(private_untypedNode);
                         ROSE_ASSERT(untypedStatement != NULL);

                         SgUntypedStatementPtrList & stmtList = untypedStatementList->get_stmt_list();

                      // untypedStatementList->get_stmt_list().insert(untypedStatementList->get_stmt_list().end(),untypedStatement);
                         stmtList.insert(stmtList.end(),untypedStatement);

                         printf ("untypedStatementList->get_stmt_list().size() = %zu \n",untypedStatementList->get_stmt_list().size());
                       }
#if 0
                    printf ("case of SgUntypedPackageDeclaration not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedScope:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedScope \n");

                    SgUntypedScope* untypedScope = isSgUntypedScope(untypedNode);
                    ROSE_ASSERT(untypedScope != NULL);

                    int element_id = getAsisAttribute(untypedScope);

                    printf ("element_id = %d \n",element_id);

                    ROSE_ASSERT(asisMap.find(element_id) != asisMap.end());
                    Element_Struct* elementStruct = asisMap[element_id];

                    ROSE_ASSERT(elementStruct != NULL);

                    printf ("elementStruct->Element_Kind = %d = %s \n",elementStruct->Element_Kind,elementKindName(elementStruct->Element_Kind).c_str());

                    SgUntypedStatementList* statementList = untypedScope->get_statement_list();
                    ROSE_ASSERT(statementList != NULL);

                 // ROSE_ASSERT(elementStruct->Element_Kind == A_Declaration);

                    if (elementStruct->Element_Kind == A_Declaration)
                       {
                         populateChildrenFromDeclaration(statementList,elementStruct->The_Union.Declaration);
                       }
                      else
                       {
                         populateChildrenFromDefinition(statementList,elementStruct->The_Union.Definition);
                       }

#if 0
                    printf ("case of SgUntypedScope not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedTaskDeclaration:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedTaskDeclaration \n");
#if 0
                    printf ("case of SgUntypedTaskDeclaration not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedLoopStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedLoopStatement \n");
#if 0
                    printf ("case of SgUntypedLoopStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedAcceptStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedAcceptStatement \n");
#if 0
                    printf ("case of SgUntypedAcceptStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedAssignmentStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedAssignmentStatement \n");
#if 0
                    printf ("case of SgUntypedAssignmentStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedRaiseStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedRaiseStatement \n");
#if 0
                    printf ("case of SgUntypedRaiseStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedEntryCallStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedEntryCallStatement \n");
#if 0
                    printf ("case of SgUntypedEntryCallStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedProcedureCallStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedProcedureCallStatement \n");
#if 0
                    printf ("case of SgUntypedProcedureCallStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedNullStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedNullStatement \n");
#if 0
                    printf ("case of SgUntypedNullStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedFunctionScope:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedFunctionScope \n");

                    SgUntypedFunctionScope* untypedFunctionScope = isSgUntypedFunctionScope(untypedNode);
                    ROSE_ASSERT(untypedFunctionScope != NULL);
                    int element_id = getAsisAttribute(untypedFunctionScope);

                    printf ("element_id = %d \n",element_id);

                 // Declaration_Struct* declarationStruct = asisMap[element_id];
                    Element_Struct* elementStruct = asisMap[element_id];
                    ROSE_ASSERT(elementStruct != NULL);

                    Element_Kinds   element_kind           = elementStruct->Element_Kind;

                    ROSE_ASSERT(element_kind == A_Declaration);

                 // Declaration_Struct* declarationStruct = asisMap[element_id];
                    Element_Union & element_union          = elementStruct->The_Union;
                    Declaration_Struct & declarationStruct = element_union.Declaration;
                 // ROSE_ASSERT(declarationStruct != NULL);

                    Statement_List & bodyStatements = declarationStruct.Body_Statements;
                    printf ("      bodyStatements: \n");
                    processStatementList(bodyStatements);

                    for (int i=0; i < bodyStatements.Length; i++)
                       {
                         int id = bodyStatements.IDs[i];

                         printf ("   --- id = %d \n",id);

                         Element_Struct* statement_elementStruct = asisMap[element_id];
                         ROSE_ASSERT(statement_elementStruct != NULL);

                         Element_Kinds   statement_element_kind           = statement_elementStruct->Element_Kind;

                         printf ("   --- --- statement_element_kind (value) = %d \n",statement_element_kind);
                         printf ("   --- --- statement_element_kind (name)  = %s \n",elementKindName(statement_element_kind).c_str());

                         ROSE_ASSERT(statement_element_kind == A_Declaration);

                      // Element_Union & statement_element_union          = statement_elementStruct->The_Union;
                      // Declaration_Struct & statement_declarationStruct = statement_element_union.Declaration;

                         SgUntypedNode* tmp_untypedNode = untypedNodeMap[id];
                         if (tmp_untypedNode != NULL)
                         {

                         SgUntypedStatement* statement_untypedNode = isSgUntypedStatement(tmp_untypedNode);
                         ROSE_ASSERT(statement_untypedNode != NULL);

                         printf ("   --- --- statement_untypedNode = %p = %s \n",statement_untypedNode,statement_untypedNode->class_name().c_str());

                         untypedFunctionScope->get_statement_list()->get_stmt_list().push_back(statement_untypedNode);
                         }
                       }

#if 0
                    printf ("case of SgUntypedFunctionScope not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedUnitDeclaration:
                  {
                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedUnitDeclaration \n");

                    SgUntypedUnitDeclaration* untypedUnitDeclaration = isSgUntypedUnitDeclaration(untypedNode);
                    ROSE_ASSERT(untypedUnitDeclaration != NULL);
                    int element_id = getAsisAttribute(untypedUnitDeclaration);

                    printf ("element_id = %d \n",element_id);

                 // Declaration_Struct* declarationStruct = asisMap[element_id];
                    Element_Struct* elementStruct = asisMap[element_id];
                    ROSE_ASSERT(elementStruct != NULL);

                 // I don't like doing this!
                    Unit_Struct* unitStruct = (Unit_Struct*) elementStruct;

                    Unit_Kinds   unit_kind           = unitStruct->Unit_Kind;

                    printf ("unit_kind = %d = %s \n",unit_kind,unitKindName(unit_kind).c_str());

                 // Not clear if we need to be restrictive on the kind.
                 // ROSE_ASSERT(unit_kind == A_Package_Body);

                 // This child is the Unit_Declaration.
                    int unit_declaration_id = unitStruct->Unit_Declaration;

                    printf ("unit_declaration_id = %d \n",unit_declaration_id);

                    ROSE_ASSERT(untypedNodeMap.find(unit_declaration_id) != untypedNodeMap.end());
                    SgUntypedNode* tmp_untypedNode = untypedNodeMap[unit_declaration_id];
                    ROSE_ASSERT(tmp_untypedNode != NULL);
#if 1
                    SgUntypedStatement* statement_untypedNode = isSgUntypedStatement(tmp_untypedNode);
                    ROSE_ASSERT(statement_untypedNode != NULL);

                    printf ("   --- --- statement_untypedNode = %p = %s \n",statement_untypedNode,statement_untypedNode->class_name().c_str());

                    SgUntypedScope* untypedScope = untypedUnitDeclaration->get_scope();
                    ROSE_ASSERT(untypedScope != NULL);

                    untypedScope->get_statement_list()->get_stmt_list().push_back(statement_untypedNode);
#else
                    printf ("   --- --- tmp_untypedNode = %p = %s \n",tmp_untypedNode,tmp_untypedNode->class_name().c_str());

                    SgUntypedScope* scope_untypedNode = isSgUntypedScope(tmp_untypedNode);
                    ROSE_ASSERT(scope_untypedNode != NULL);

                    untypedUnitDeclaration->set_scope(scope_untypedNode);
                    ROSE_ASSERT(untypedUnitDeclaration->get_scope() != NULL);
#endif

#if 0
                 // Declaration_Struct* declarationStruct = asisMap[element_id];
                    Element_Union & element_union          = elementStruct->The_Union;
                    Declaration_Struct & declarationStruct = element_union.Declaration;
                 // ROSE_ASSERT(declarationStruct != NULL);

                    Statement_List & bodyStatements = declarationStruct.Body_Statements;
                    printf ("      bodyStatements: \n");
                    processStatementList(bodyStatements);

                    for (int i=0; i < bodyStatements.Length; i++)
                       {
                         int id = bodyStatements.IDs[i];

                         printf ("   --- id = %d \n",id);

                         Element_Struct* statement_elementStruct = asisMap[element_id];
                         ROSE_ASSERT(statement_elementStruct != NULL);

                         Element_Kinds   statement_element_kind           = statement_elementStruct->Element_Kind;

                         printf ("   --- --- statement_element_kind (value) = %d \n",statement_element_kind);
                         printf ("   --- --- statement_element_kind (name)  = %s \n",elementKindName(statement_element_kind).c_str());

                         ROSE_ASSERT(statement_element_kind == A_Declaration);

                      // Element_Union & statement_element_union          = statement_elementStruct->The_Union;
                      // Declaration_Struct & statement_declarationStruct = statement_element_union.Declaration;

                         SgUntypedNode* tmp_untypedNode = untypedNodeMap[id];
                         ROSE_ASSERT(tmp_untypedNode != NULL);

                         SgUntypedStatement* statement_untypedNode = isSgUntypedStatement(tmp_untypedNode);
                         ROSE_ASSERT(statement_untypedNode != NULL);

                         printf ("   --- --- statement_untypedNode = %p = %s \n",statement_untypedNode,statement_untypedNode->class_name().c_str());

                         untypedFunctionScope->get_statement_list()->get_stmt_list().push_back(statement_untypedNode);
                       }
#endif
#if 0
                    printf ("case of SgUntypedUnitDeclaration not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedReturnStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedReturnStatement \n");
#if 0
                    printf ("case of SgUntypedReturnStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedExpression:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedExpression \n");
#if 0
                    printf ("case of SgUntypedExpression not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedExceptionHandlerDeclaration:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedExceptionHandlerDeclaration \n");
#if 0
                    printf ("case of SgUntypedExceptionHandlerDeclaration not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedExceptionDeclaration:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedExceptionDeclaration \n");
#if 0
                    printf ("case of SgUntypedExceptionDeclaration not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

             case V_SgUntypedIfStatement:
                  {
                 // This is a child nodes and we only visit parents to connect up children, so nothing to do here.

                 // Find the parent element and connect this into the list of children.
                    printf ("Found a SgUntypedIfStatement \n");

                 // Statement_List & sequenceOfStatements = path.Sequence_Of_Statements;
                 // printf ("      sequenceOfStatements: \n");
                 // processElementList(sequenceOfStatements);

                    SgUntypedIfStatement* untypedIfStatement = isSgUntypedIfStatement(untypedNode);
                    ROSE_ASSERT(untypedIfStatement != NULL);
                    int element_id = getAsisAttribute(untypedIfStatement);

                    printf ("element_id = %d \n",element_id);

                 // Declaration_Struct* declarationStruct = asisMap[element_id];
                    Element_Struct* elementStruct = asisMap[element_id];
                    ROSE_ASSERT(elementStruct != NULL);

                    Element_Kinds   element_kind           = elementStruct->Element_Kind;

                    ROSE_ASSERT(element_kind == A_Statement);

                 // Declaration_Struct* declarationStruct = asisMap[element_id];
                    Element_Union    & element_union   = elementStruct->The_Union;
                    Statement_Struct & statementStruct = element_union.Statement;

                    Statement_Kinds statementKind = statementStruct.Statement_Kind;
                    ROSE_ASSERT(statementKind == An_If_Statement);

                    Path_List & pathList = statementStruct.Statement_Paths;
                    ROSE_ASSERT(pathList.Length >= 1);
                    ROSE_ASSERT(pathList.Length <= 2);

                    int true_path_element_id = pathList.IDs[0];

                    printf ("true_path_element_id = %d \n",true_path_element_id);

                    ROSE_ASSERT(untypedNodeMap.find(true_path_element_id) != untypedNodeMap.end());
                    SgUntypedNode* true_body_untypedNode = untypedNodeMap[true_path_element_id];
                    ROSE_ASSERT(true_body_untypedNode != NULL);

                    SgUntypedScope* true_body_untypedScope = isSgUntypedScope(true_body_untypedNode);
                    ROSE_ASSERT(true_body_untypedScope != NULL);

                    untypedIfStatement->set_true_body(true_body_untypedScope);

                    if (pathList.Length > 1)
                       {
                         ROSE_ASSERT(pathList.Length == 2);
                         int false_path_element_id = pathList.IDs[1];
                         printf ("false_path_element_id = %d \n",false_path_element_id);

                         ROSE_ASSERT(untypedNodeMap.find(false_path_element_id) != untypedNodeMap.end());
                         SgUntypedNode* false_body_untypedNode = untypedNodeMap[false_path_element_id];
                         ROSE_ASSERT(false_body_untypedNode != NULL);

                         SgUntypedScope* false_body_untypedScope = isSgUntypedScope(false_body_untypedNode);
                         ROSE_ASSERT(false_body_untypedScope != NULL);

                         untypedIfStatement->set_false_body(false_body_untypedScope);
                       }
#if 0
                    Statement_List & bodyStatements = statementStruct.Statement_Paths.IDs[0];
                    printf ("      bodyStatements: \n");
                    processStatementList(bodyStatements);

                    for (int i=0; i < bodyStatements.Length; i++)
                       {
                         int id = bodyStatements.IDs[i];

                         printf ("   --- id = %d \n",id);

                         Element_Struct* statement_elementStruct = asisMap[element_id];
                         ROSE_ASSERT(statement_elementStruct != NULL);

                         Element_Kinds   statement_element_kind           = statement_elementStruct->Element_Kind;

                         printf ("   --- --- statement_element_kind (value) = %d \n",statement_element_kind);
                         printf ("   --- --- statement_element_kind (name)  = %s \n",elementKindName(statement_element_kind).c_str());

                         ROSE_ASSERT(statement_element_kind == A_Declaration);

                      // Element_Union & statement_element_union          = statement_elementStruct->The_Union;
                      // Declaration_Struct & statement_declarationStruct = statement_element_union.Declaration;

                         SgUntypedNode* tmp_untypedNode = untypedNodeMap[id];
                         ROSE_ASSERT(tmp_untypedNode != NULL);

                         SgUntypedStatement* statement_untypedNode = isSgUntypedStatement(tmp_untypedNode);
                         ROSE_ASSERT(statement_untypedNode != NULL);

                         printf ("   --- --- statement_untypedNode = %p = %s \n",statement_untypedNode,statement_untypedNode->class_name().c_str());

                      // untypedIfStatement->get_statement_list()->get_stmt_list().push_back(statement_untypedNode);
                       }
#endif

#if 0
                    printf ("case of SgUntypedIfStatement not supported yet! \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }

            // Trap all other cases.
               default:
                  {
                    printf ("Error: default reached in switch: untypedNode in untypedNodeMap is: %p %s \n",untypedNode,untypedNode->class_name().c_str());
                    //~ ROSE_ASSERT(false);
                  }
             }

        }

#if 1
     printf ("DONE: Connect up the SgUntypedNodes for form an AST \n");
#endif

#if 1
     printf ("Test traversal over untyped AST \n");
#endif

     //~ testTraversal();

#if 1
     printf ("DONE: Test traversal over untyped AST \n");
#endif

#if 1
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
  // generateDOT (*project);
  // generateDOT (globalUntypedFile);
  // AstDOTGeneration astdotgen;

#if 1
     printf ("Test traversal over untyped AST \n");
#endif

  // astdotgen.generateInputFiles(globalUntypedFile,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,filenamePostfix);
  // void generate(SgNode* node,  std::string filename, traversalType tt = TOPDOWNBOTTOMUP,std::string filenamePostfix = "");
  // astdotgen.generate(globalUntypedFile, "untypedAdaAST");
  // astdotgen.generate(globalUntypedFile, "ada",AstDOTGeneration::TOPDOWNBOTTOMUP,"untypedAst");
     //~ generateDOT(globalUntypedFile,"adaUntypedAst");

#if 1
     printf ("DONE: Test traversal over untyped AST \n");
#endif

#endif


#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

     secondConversion(head_nodes, file);
   }



void
Ada_ROSE_Translation::populateChildrenFromDeclaration(SgUntypedStatementList* statementList, Declaration_Struct & declaration)
   {

     Declaration_Kinds declarationKind = declaration.Declaration_Kind;

     ROSE_ASSERT(statementList != NULL);

     switch (declarationKind)
        {
          case A_Package_Body_Declaration:
          case A_Task_Body_Declaration:
       // case An_Entry_Body_Declaration:
             {
               Element_ID_List        & bodyDeclarativeItems  = declaration.Body_Declarative_Items;
               printf ("      bodyDeclarativeItems: \n");
               processElementList(bodyDeclarativeItems);
               Statement_List         & bodyStatements        = declaration.Body_Statements;
               printf ("      bodyStatements: \n");
               processStatementList(bodyStatements);
               Exception_Handler_List & bodyExceptionHandlers = declaration.Body_Exception_Handlers;
               printf ("      bodyExceptionHandlers: \n");
               processExceptionHandlerList(bodyExceptionHandlers);
               Declaration_ID           bodyBlockStatement    = declaration.Body_Block_Statement;
               printf ("      bodyBlockStatement = %d \n",bodyBlockStatement);

               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("      isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               bool  isSubunit = declaration.Is_Subunit;
               printf ("      isSubunit = %s \n",isSubunit ? "true" : "false");
               Declaration_ID correspondingBodyStub = declaration.Corresponding_Body_Stub;
               printf ("      correspondingBodyStub = %d \n",correspondingBodyStub);

            // Process the bodyDeclarativeItems as children.
               for (int i = 0; i < bodyDeclarativeItems.Length; i++)
                  {
                    int id = bodyDeclarativeItems.IDs[i];
                    printf ("   --- i = %d \n",i);

                    ROSE_ASSERT(untypedNodeMap.find(id) != untypedNodeMap.end());

                    SgUntypedNode* untypedNode = untypedNodeMap[id];
                    ROSE_ASSERT(untypedNode != NULL);

                    printf ("   --- untypedNode = %p = %s \n",untypedNode,untypedNode->class_name().c_str());

                    SgUntypedStatement* untypedStatement = isSgUntypedStatement(untypedNode);
                    ROSE_ASSERT(untypedStatement != NULL);

                    statementList->get_stmt_list().push_back(untypedStatement);
                  }


// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Package_Declaration:
             {
               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declarative_Item_List & visiblePartDeclarativeItems  = declaration.Visible_Part_Declarative_Items;
               printf ("visiblePartDeclarativeItems: \n");
               processDeclarativeItemList(visiblePartDeclarativeItems);

               bool                    isPrivatePresent              = declaration.Is_Private_Present;
               printf ("      isPrivatePresent = %s \n",isPrivatePresent ? "true" : "false");

               Declarative_Item_List & privatePartDeclarativeItems  = declaration.Private_Part_Declarative_Items;
               printf ("privatePartDeclarativeItems: \n");
               processDeclarativeItemList(privatePartDeclarativeItems);

            // ********************************
            // This needs to force the declaration of a function (with function definition, and body).
               printf ("In processDeclaration(): case A_Package_Declaration: (not implemented) \n");

               Defining_Name_List & names = declaration.Names;

            // Get the name from the name list.
               ROSE_ASSERT(names.Length == 1);
               int packageName_id = names.IDs[0];

               printf ("packageName_id = %d \n",packageName_id);

               ROSE_ASSERT(untypedNodeMap.find(packageName_id) != untypedNodeMap.end());
               SgUntypedNode* untypedNode = untypedNodeMap[packageName_id];
               ROSE_ASSERT(untypedNode != NULL);
            // SgUntypedInitializedName* untypedInitializedName = isSgUntypedInitializedName(untypedNode);
            // ROSE_ASSERT(untypedInitializedName != NULL);
            // string packageName = untypedInitializedName->get_name();
               SgUntypedName* untypedName = isSgUntypedName(untypedNode);
               ROSE_ASSERT(untypedName != NULL);
               string packageName = untypedName->get_name();

               printf ("packageName = %s \n",packageName.c_str());

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               printf ("Default reached in populateChildrenFromDeclaration(): declarationKind = %d declarationKind = %s \n",declarationKind,declarationKindName(declarationKind).c_str());
               ROSE_ASSERT(false);
             }
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::populateChildrenFromDefinition( SgUntypedStatementList* statementList, Definition_Struct & definition)
   {
     ROSE_ASSERT(statementList != NULL);

     Definition_Kinds definitionKind = definition.Definition_Kind;

     printf ("   In populateChildrenFromDefinition(): \n");
     printf ("      definitionKind (value) = %d \n",definitionKind);
     printf ("      definitionKind (name)  = %s \n",definitionKindName(definitionKind).c_str());

     switch (definitionKind)
        {
          case Not_A_Definition:
             {
               printf ("Error: switch has case Not_A_Definition \n");
               ROSE_ASSERT(false);
               break;
             }

          case A_Type_Definition:
             {
            // Ignoring type related child nodes (focus first on statements and then expressions).

            // processTypeDefinition(definition.The_Union.The_Type_Definition);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Subtype_Indication:
             {
            // Ignoring type related child nodes (focus first on statements and then expressions).

            // processSubtypeIndication(definition.The_Union.The_Subtype_Indication);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Constraint:
             {
            // processConstraint(definition.The_Union.The_Constraint);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Component_Definition:
             {
            // Ignoring type related child nodes (focus first on statements and then expressions).

            // processComponentDefinition(definition.The_Union.The_Component_Definition);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Discrete_Subtype_Definition:
             {
            // processDiscreteSubtypeDefinition(definition.The_Union.The_Discrete_Subtype_Definition);
#if 1
               printf ("Exiting as a test! \n");
               //~ ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Discrete_Range:
             {
            // processDiscreteRange(definition.The_Union.The_Discrete_Range);
#if 1
               printf ("Exiting as a test! \n");
               // ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Unknown_Discriminant_Part:
             {
            // processUnknownDiscriminantPart(definition.The_Union.The_Unknown_Discriminant_Part);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Known_Discriminant_Part:
             {
            // processKnownDiscriminantPart(definition.The_Union.The_Known_Discriminant_Part);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Record_Definition:
             {
            // Ignoring type related child nodes (focus first on statements and then expressions).

            // processRecordDefinition(definition.The_Union.The_Record_Definition);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Null_Record_Definition:
             {
            // processNullRecordDefinition(definition.The_Union.The_Null_Record_Definition);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Null_Component:
             {
            // processNullComponent(definition.The_Union.The_Null_Component);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Variant_Part:
             {
            // processVariantPart(definition.The_Union.The_Variant_Part);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Variant:
             {
            // processVariant(definition.The_Union.The_Variant);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Others_Choice:
             {
            // processOthersChoice(definition.The_Union.The_Others_Choice);
#if 1
               printf ("Exiting as a test! \n");
               // ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Access_Definition:
             {
            // The information in this node is captured when the SgUntypedName is constructed.

            // processAccessDefinition(definition.The_Union.The_Access_Definition);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Private_Type_Definition:
             {
            // processPrivateTypeDefinition(definition.The_Union.The_Private_Type_Definition);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Tagged_Private_Type_Definition:
             {
            // processTaggedPrivateTypeDefinition(definition.The_Union.The_Tagged_Private_Type_Definition);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Private_Extension_Definition:
             {
            // processPrivateExtensionDefinition(definition.The_Union.The_Private_Extension_Definition);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Task_Definition:
             {
            // This is not a node that has children (as I presently understand it).

            // processTaskDefinition(definition.The_Union.The_Task_Definition);
#if 0
            // Process the bodyBlockStatements as children.
               for (int i = 0; i < bodyBlockStatements.Length; i++)
                  {
                    int id = bodyBlockStatements.IDs[i];
                    printf ("   --- i = %d \n",i);

                    ROSE_ASSERT(untypedNodeMap.find(id) != untypedNodeMap.end());

                    SgUntypedNode* untypedNode = untypedNodeMap[id];
                    ROSE_ASSERT(untypedNode != NULL);

                    printf ("   --- untypedNode = %p = %s \n",untypedNode,untypedNode->class_name().c_str());

                    SgUntypedStatement* untypedStatement = isSgUntypedStatement(untypedNode);
                    ROSE_ASSERT(untypedStatement != NULL);

                    statementList->get_stmt_list().push_back(untypedStatement);
                  }
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Protected_Definition:
             {
            // processProtectedDefinition(definition.The_Union.The_Protected_Definition);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Formal_Type_Definition:
             {
            // processFormalTypeDefinition(definition.The_Union.The_Formal_Type_Definition);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Aspect_Specification:
             {
            // processAspectSpecification(definition.The_Union.The_Aspect_Specification);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               printf ("Default reached in populateChildrenFromDefinition(): definitionKind = %d definitionKind = %s \n",definitionKind,definitionKindName(definitionKind).c_str());
               ROSE_ASSERT(false);
             }
        }

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::testTraversal()
   {
  // Test the traversal of the untyped AST
     class Traversal : public AstSimpleProcessing
        {
          public:
               void visit(SgNode* n)
                  {
                    if (n != NULL)
                       {
                         printf ("In traversal: n = %p = %s \n",n,n->class_name().c_str());
                       }
                      else
                       {
                         printf ("In traversal: n = %p \n",n);
                       }
                  }
        };

#if 1
     printf ("In testTraversal(): Test traversal over untyped AST \n");
#endif

     Traversal traverse;
     traverse.traverse(globalUntypedFile,preorder);

#if 1
     printf ("In testTraversal(): DONE: Test traversal over untyped AST \n");
#endif
   }

// void Ada_ROSE_Translation::processContext (Node_Union & nodeUnion)
void
Ada_ROSE_Translation::processContext (Context_Struct & context)
   {
  // For Ada default initialization (ignored).
  // int & dummyMember = nodeUnion.Dummy_Member;

     printf ("In Ada_ROSE_Translation::processContext() \n");

  // Context_Struct & context = nodeUnion.Context;
  // Unit_Struct    & unit    = nodeUnion.Unit;
  // Element_Struct & element = nodeUnion.Element;

#if 0
     char *name;
     char *parameters;
     char *debug_image;
#endif

     string contextName       = context.name;
     string contextParameters = context.parameters;
     string contextDebugImage = context.debug_image;

     printf ("contextName       = %s \n",contextName.c_str());
     printf ("contextParameters = %s \n",contextParameters.c_str());
     printf ("contextDebugImage = %s \n",contextDebugImage.c_str());

   }

void
Ada_ROSE_Translation::processUnit (Unit_Structs_Ptr unitList)
   {

  // For Ada default initialization (ignored).
  // int & dummyMember = nodeUnion.Dummy_Member;

     printf ("In Ada_ROSE_Translation::processUnit() \n");

  // Context_Struct & context = nodeUnion.Context;
  // Unit_Struct    & unit    = nodeUnion.Unit;
     Unit_Struct    & unit    = unitList->Unit;
  // Element_Struct & element = nodeUnion.Element;
#if 0
     printf ("In Ada_ROSE_Translation::processUnit(): after reference to Unit_Struct \n");
#endif

#if 0
     Unit_ID         ID;
     enum Unit_Kinds   Unit_Kind;
     enum Unit_Classes Unit_Class;
     enum Unit_Origins Unit_Origin;
  // Enclosing_Context
  // Enclosing_Container
     Unit_List         Corresponding_Children;
     Unit_ID           Corresponding_Parent_Declaration;
     Unit_ID           Corresponding_Declaration;
     Unit_ID           Corresponding_Body;
     char             *Unit_Full_Name; // Ada name
     char             *Unique_Name; // file name etc.
     bool              Exists;
     bool              Can_Be_Main_Program;
     bool              Is_Body_Required;
     char             *Text_Name;
     char             *Text_Form;
     char             *Object_Name;
     char             *Object_Form;
     char             *Compilation_Command_Line_Options;
     Unit_List         Subunits;
     Unit_ID           Corresponding_Subunit_Parent_Body;
     char             *Debug_Image;
     Declaration_ID      Unit_Declaration;
     Context_Clause_List Context_Clause_Elements;
     Pragma_Element_List Compilation_Pragmas;
#endif

  // Order of appearance in compilation unit:
  //    1) Compilation_Pragmas
  //    2) Context_Clause_Elements
  //    3) Unit_Declaration

#if 0
     printf ("In Ada_ROSE_Translation::processUnit(): process integer \n");
#endif

     Unit_ID        unit_id      = unit.ID;
#if 0
     printf ("In Ada_ROSE_Translation::processUnit(): process enum \n");
#endif

     Unit_Kinds     unit_kind   = unit.Unit_Kind;
     Unit_Classes   unit_class  = unit.Unit_Class;
     Unit_Origins   unit_origin = unit.Unit_Origin;

  // Enclosing_Context
  // Enclosing_Container
     Unit_List    & correspondingChildren        = unit.Corresponding_Children;

#if 0
     printf ("In Ada_ROSE_Translation::processUnit(): process remaining data members \n");
#endif

     Unit_ID      correspondingParentDeclaration = unit.Corresponding_Parent_Declaration;
     Unit_ID      correspondingDeclaration       = unit.Corresponding_Declaration;
     Unit_ID      correspondingBody              = unit.Corresponding_Body;
     string       unitFullName                   = unit.Unit_Full_Name; // Ada name
     string       uniqueName                     = unit.Unique_Name; // file name etc.
     bool         exists                         = unit.Exists;
     bool         isMainProgram                  = unit.Can_Be_Main_Program;
     bool         isBodyRequired                 = unit.Is_Body_Required;
     string       textName                       = unit.Text_Name;

#if 0
  // DQ (9/13/2017): All of these have been tested seperately and each of them fail.
     printf ("In Ada_ROSE_Translation::processUnit(): test 9 \n");

  // string       testForm                       = unit.Text_Form;

     printf ("In Ada_ROSE_Translation::processUnit(): test 10 \n");

  // string       objectName                     = unit.Object_Name;

     printf ("In Ada_ROSE_Translation::processUnit(): test 11 \n");

  // string       objectForm                     = unit.Object_Form;

     printf ("In Ada_ROSE_Translation::processUnit(): test 12 \n");

  // string       compilationCommandLineOptions  = unit.Compilation_Command_Line_Options;
#endif

     Unit_List  & subunits                       = unit.Subunits;
     Unit_ID      correspondingSubunitParentBody = unit.Corresponding_Subunit_Parent_Body;
     string       debugImage                     = unit.Debug_Image;

#if 0
     printf ("In Ada_ROSE_Translation::processUnit(): DONE: process remaining data members \n");
#endif

     printf ("In processUnit(): \n");
     printf ("   unit_id                        = %d \n",unit_id);
     printf ("   unit_kind (value)              = %d \n",unit_kind);
     printf ("   unit_kind (name)               = %s \n",unitKindName(unit_kind).c_str());
     printf ("   unit_class (value)             = %d \n",unit_class);
     printf ("   unit_class (name)              = %s \n",unitClassName(unit_class).c_str());
     printf ("   unit_origin (value)            = %d \n",unit_origin);
     printf ("   unit_origin (name)             = %s \n",unitOriginName(unit_origin).c_str());

  // Missing unit_classes
  // Missing unit_origins
  // Missing correspondingChildren
     printf ("correspondingChildren: \n");
     processUnitList(correspondingChildren);

     printf ("   correspondingParentDeclaration = %d \n",correspondingParentDeclaration);
     printf ("   correspondingDeclaration       = %d \n",correspondingDeclaration);
     printf ("   correspondingBody              = %d \n",correspondingBody);
     printf ("   unitFullName                   = %s \n",unitFullName.c_str());
     printf ("   uniqueName                     = %s \n",uniqueName.c_str());
     printf ("   exists                         = %s \n",exists ? "true" : "false");
     printf ("   isMainProgram                  = %s \n",isMainProgram ? "true" : "false");
     printf ("   isBodyRequired                 = %s \n",isBodyRequired ? "true" : "false");
     printf ("   textName                       = %s \n",textName.c_str());

#if 0
  // printf ("   testForm                       = %s \n",testForm.c_str());
  // printf ("   objectName                     = %s \n",objectName.c_str());
  // printf ("   objectForm                     = %s \n",objectForm.c_str());
  // printf ("   compilationCommandLineOptions  = %s \n",compilationCommandLineOptions.c_str());
#endif

  // Missing subunits
     printf ("subunits: \n");
     processUnitList(subunits);

     printf ("   correspondingSubunitParentBody = %d \n",correspondingSubunitParentBody);
     printf ("   debugImage                     = %s \n",debugImage.c_str());

  // Declaration_ID      Unit_Declaration;
     printf ("   Unit_Declaration = %d \n",unit.Unit_Declaration);

  // Context_Clause_List Context_Clause_Elements;
     printf ("   Context_Clause_Elements: \n");
     processContentClauseList ( unit.Context_Clause_Elements);

  // Pragma_Element_List Compilation_Pragmas;
     printf ("   Compilation_Pragmas: \n");
     processPragmaElementList ( unit.Compilation_Pragmas);

     if (globalUntypedFile == NULL)
        {
          SgUntypedGlobalScope* untypedGlobalScope = new SgUntypedGlobalScope();
          ROSE_ASSERT(untypedGlobalScope != NULL);

       // Add the statement list to the global scope (the constructor preinitialization
       // should do this (so this should be fixed in the general untyped node support).
          SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
          ROSE_ASSERT(untypedStatementList != NULL);
          untypedGlobalScope->set_statement_list(untypedStatementList);

          ROSE_ASSERT(untypedGlobalScope->get_statement_list() != NULL);

          SgUntypedFile* untypedFile = new SgUntypedFile(untypedGlobalScope);
          ROSE_ASSERT(untypedFile != NULL);

          globalUntypedFile = untypedFile;
        }
       else
        {
          printf ("SgUntypedFile node has previously been built! \n");
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 1
  // This will build a scope in the unit declaration.

  // Add the statement list to the global scope (the constructor preinitialization
  // should do this (so this should be fixed in the general untyped node support).
     SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
     ROSE_ASSERT(untypedStatementList != NULL);

     SgUntypedScope* untypedScope = new SgUntypedScope();
     ROSE_ASSERT(untypedScope != NULL);

     untypedScope->set_statement_list(untypedStatementList);
     ROSE_ASSERT(untypedScope->get_statement_list() != NULL);

#if 0
     SgUntypedFunctionDeclarationList* untypedFunctionDeclarationList = new SgUntypedFunctionDeclarationList();
     ROSE_ASSERT(untypedFunctionDeclarationList != NULL);

     untypedScope->set_function_list(untypedFunctionDeclarationList);
     ROSE_ASSERT(untypedScope->get_function_list() != NULL);
#endif

     printf ("untypedScope in SgUntypedUnitDeclaration = %p = %s \n",untypedScope,untypedScope->class_name().c_str());
     printf ("untypedScope->get_statement_list() = %p \n",untypedScope->get_statement_list());
     printf ("untypedScope->get_function_list()  = %p \n",untypedScope->get_function_list());

     printf ("untypedStatementList->get_stmt_list().size() = %zu \n",untypedStatementList->get_stmt_list().size());

     SgUntypedUnitDeclaration* untypedUnitDeclaration = new SgUntypedUnitDeclaration();
     ROSE_ASSERT(untypedUnitDeclaration != NULL);

     untypedUnitDeclaration->set_scope(untypedScope);
     ROSE_ASSERT(untypedUnitDeclaration->get_scope() != NULL);
#else
  // We want to also build the scope in the unit declaration (so we want the code above).
  // We want to use the scoep build in another rule to avoid redundent scope handling.
     SgUntypedUnitDeclaration* untypedUnitDeclaration = new SgUntypedUnitDeclaration();
     ROSE_ASSERT(untypedUnitDeclaration != NULL);
#endif

  // Not clear if this or the unique name is best to use here (this is more representative of the structure).
     untypedUnitDeclaration->set_name(unitFullName);

  // DQ (10/15/2017): We need a concept of element_id to put the unit into the maps.
  // int id = unit_id;
     printf ("Using unit_id for element_id in maps: unit_id = %d \n",unit_id);

  // Note that zero is reserved for the SgUntypedFile IR node in out map (units start at unit_id == 1)
     ROSE_ASSERT(unit_id > 0);

     printf ("Processing untypedUnitDeclaration = %p = %s unit_id = %d \n",untypedUnitDeclaration,untypedUnitDeclaration->get_name().c_str(),unit_id);

     processUntypedNode(untypedUnitDeclaration,unit_id);

  // We don't need an attribute for this SgUntypedFile.
  // Note that the value (zero) is not significant because we don't use the value to look up the unit.
  // addAsisAttribute (globalUntypedFile,0);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


// void Ada_ROSE_Translation::processElement (Node_Union & nodeUnion)
void
Ada_ROSE_Translation::processElement (Element_Structs_Ptr elementList)
   {
  // For Ada default initialization (ignored).
  // int & dummyMember = nodeUnion.Dummy_Member;

  // Context_Struct & context = nodeUnion.Context;
  // Unit_Struct    & unit    = nodeUnion.Unit;

#if 0
     printf ("In Ada_ROSE_Translation::processElement() \n");
#endif

  // Element_Struct & element = nodeUnion.Element;
     Element_Struct & element = elementList->Element;

     Element_ID      element_id             = element.ID;
     Element_Kinds   element_kind           = element.Element_Kind;

#if 0
  // Limit the output to be more manageable.
     Node_ID         enclosing_element_id   = element.Enclosing_Element_ID;
     Enclosing_Kinds enclosing_element_kind = element.Enclosing_Kind;
#endif

  // string          source_location        = element.Source_Location;
     Source_Location_Struct & source_location = element.Source_Location;

     Element_Union & element_union          = element.The_Union;

     printf ("\n\nIn processElement(): \n");
     printf ("   element_id                     = %d \n",element_id);
     printf ("   element_kind (vale)            = %d \n",element_kind);
     printf ("   element_kind (name)            = %s \n",elementKindName(element_kind).c_str());

#if 0
  // Limit the output to be more manageable.
     printf ("   enclosing_element_id           = %d \n",enclosing_element_id);
     printf ("   enclosing_element_kind (value) = %d \n",enclosing_element_kind);
     printf ("   enclosing_element_kind (name)  = %s \n",enclosingKindName(enclosing_element_kind).c_str());
#endif

  // printf ("   source_location                = %s \n",source_location.c_str());
     printf ("   source_location (file = %s line:col = %d:%d - %d:%d) \n",source_location.Unit_Name,source_location.First_Line,source_location.First_Column,source_location.Last_Line,source_location.Last_Column);

     switch (element_kind)
        {
          case Not_An_Element:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case Not_An_Element: not implemented \n");
               ROSE_ASSERT(false);
               break;
             }

          case A_Pragma:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case A_Pragma: not implemented \n");
               ROSE_ASSERT(false);
               break;
             }

          case A_Defining_Name:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case A_Defining_Name: not implemented \n");

               Defining_Name_Struct & definingName = element_union.Defining_Name;
               processDefiningName( definingName,element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Declaration:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case A_Declaration: not implemented \n");

               Declaration_Struct & declaration = element_union.Declaration;
               processDeclaration( declaration,element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Definition:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case A_Definition: not implemented \n");

               Definition_Struct & definition = element_union.Definition;
               processDefinition(definition,element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Expression:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case An_Expression: not implemented \n");

               Expression_Struct & expression = element_union.Expression;
               processExpression( expression, element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Association:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case An_Association: not implemented \n");

               Association_Struct & association = element_union.Association;
               processAssociation( association, element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Statement:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case A_Statement: not implemented \n");

               Statement_Struct & statement = element_union.Statement;
               processStatement(statement,element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Path:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case A_Path: not implemented \n");

               bool skipProcessing = false;
               if (untypedNodeMap.find(element_id) != untypedNodeMap.end())
                  {
                    printf ("ERROR: Element id = %d is has already been processed (skipping call to processPath()) \n",element_id);
                 // ROSE_ASSERT(false);

                    skipProcessing = true;
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               if (skipProcessing == false)
                  {
                    Path_Struct & path = element_union.Path;
                    processPath(path,element_id);
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Clause:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case A_Clause: not implemented \n");

               Clause_Struct & clause = element_union.Clause;
               processClause(clause);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Exception_Handler:
             {
               printf ("   In Ada_ROSE_Translation::processElement(): case An_Exception_Handler: not implemented \n");

               Exception_Handler_Struct & exceptionHandler = element_union.Exception_Handler;
               processExceptionHandler(exceptionHandler, element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::elementKindName(): element_kind = %d \n",element_kind);
               ROSE_ASSERT(false);
             }
        }

#if 0
     if (element_id == 2457)
        {
          printf ("Found target element_id = %d \n",element_id);

          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
        }
#endif
   }


void
Ada_ROSE_Translation::processStatement( Statement_Struct & statement, int element_id)
   {
  // Note that we need to seperately pass the element_id into this function because it
  // is only known to the processElement() function that is calling the processStatement()
  // function, and we can't access it from Statement_Struct because the current C language
  // data structures only permit a "Has-a" relationship instead of an "Is-a" relationship.
  // A future move to support a C++ data structure would better permit an improved design.

#if 0
  // Documentation for Statement_Struct.
     enum Statement_Kinds   Statement_Kind;
     Defining_Name_List     Label_Names;

  // These fields are only valid for the kinds above them:
  // An_Assignment_Statement,             // 5.2
     Expression_ID          Assignment_Variable_Name;
     Expression_ID          Assignment_Expression;
  //   An_If_Statement,                     // 5.3
  //   A_Case_Statement,                    // 5.4
  //   A_Selective_Accept_Statement,        // 9.7.1
  //   A_Timed_Entry_Call_Statement,        // 9.7.2
  //   A_Conditional_Entry_Call_Statement,  // 9.7.3
  //   An_Asynchronous_Select_Statement,    // 9.7.4
     Path_List              Statement_Paths;
  //   A_Case_Statement,                    // 5.4
     Expression_ID          Case_Expression;
  //   A_Loop_Statement,                    // 5.5
  //   A_While_Loop_Statement,              // 5.5
  //   A_For_Loop_Statement,                // 5.5
  //   A_Block_Statement,                   // 5.6
     Defining_Name_ID       Statement_Identifier;
  //   A_Loop_Statement,                    // 5.5
  //   A_Block_Statement,                   // 5.6
  //   An_Accept_Statement,                 // 9.5.2
     bool                   Is_Name_Repeated;
  //   A_While_Loop_Statement,              // 5.5
     Expression_ID          While_Condition;
  //   A_For_Loop_Statement,                // 5.5
     Declaration_ID         For_Loop_Parameter_Specification;
  //   A_Loop_Statement,                    // 5.5
  //   A_While_Loop_Statement,              // 5.5
  //   A_For_Loop_Statement,                // 5.5
     Statement_List         Loop_Statements;
  //   A_Block_Statement,                   // 5.6
     bool                   Is_Declare_Block;
     Declarative_Item_List  Block_Declarative_Items;
     Statement_List         Block_Statements;
     Exception_Handler_List Block_Exception_Handlers;
  //   An_Exit_Statement,                   // 5.7
     Expression_ID          Exit_Loop_Name;
     Expression_ID          Exit_Condition;
     Expression_ID          Corresponding_Loop_Exited;
  //   A_Goto_Statement,                    // 5.8
     Expression_ID          Goto_Label;
     Statement_ID           Corresponding_Destination_Statement;
  //   A_Procedure_Call_Statement,          // 6.4
  //   An_Entry_Call_Statement,             // 9.5.3
     Expression_ID          Called_Name;
     Declaration_ID         Corresponding_Called_Entity;
     Association_List       Call_Statement_Parameters;
  //   A_Return_Statement,                  // 6.5
     Expression_ID          Return_Expression;
  //   //  //|A2005 start
  //   An_Extended_Return_Statement,        // 6.5
     Declaration_ID         Return_Object_Declaration;
     Statement_List         Extended_Return_Statements;
     Exception_Handler_List Extended_Return_Exception_Handlers;
  //   //  //|A2005 end
  //   An_Accept_Statement,                 // 9.5.2
     Expression_ID          Accept_Entry_Index;
     Name_ID                Accept_Entry_Direct_Name;
     Parameter_Specification_List
                         Accept_Parameters;
     Statement_List         Accept_Body_Statements;
     Statement_List         Accept_Body_Exception_Handlers;
     Declaration_ID         Corresponding_Entry;
  //   A_Requeue_Statement,                 // 9.5.4
  //   A_Requeue_Statement_With_Abort,      // 9.5.4
     Name_ID                Requeue_Entry_Name;
  //   A_Delay_Until_Statement,             // 9.6
  //   A_Delay_Relative_Statement,          // 9.6
     Expression_ID          Delay_Expression;
  //   An_Abort_Statement,                  // 9.8
     Expression_List        Aborted_Tasks;
  //   A_Raise_Statement,                   // 11.3
     Expression_ID          Raised_Exception;
     Expression_ID          Associated_Message;
  //   A_Code_Statement                     // 13.8
     Expression_ID          Qualified_Expression;
#endif

     Statement_Kinds statementKind = statement.Statement_Kind;

     printf ("   In processStatement(): \n");
     printf ("      statementKind (value) = %d \n",statementKind);
     printf ("      statementKind (name)  = %s \n",statementKindName(statementKind).c_str());

  // Defining_Name_List     Label_Names;
     processDefiningNameList(statement.Label_Names);

     switch (statementKind)
        {
       // An_Assignment_Statement,             // 5.2
          case An_Assignment_Statement:
             {
            // Expression_ID          Assignment_Variable_Name;
            // Expression_ID          Assignment_Expression;
               printf ("Assignment_Variable_Name = %d \n",statement.Assignment_Variable_Name);
               printf ("Assignment_Expression = %d \n",statement.Assignment_Expression);

               SgUntypedAssignmentStatement* untypedStatement = new SgUntypedAssignmentStatement();

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedStatement;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedStatement,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_If_Statement,                     // 5.3
       // A_Case_Statement,                    // 5.4
       // A_Selective_Accept_Statement,        // 9.7.1
       // A_Timed_Entry_Call_Statement,        // 9.7.2
       // A_Conditional_Entry_Call_Statement,  // 9.7.3
       // An_Asynchronous_Select_Statement,    // 9.7.4
          case An_If_Statement:
       // case A_Case_Statement:
          case A_Selective_Accept_Statement:
          case A_Timed_Entry_Call_Statement:
          case A_Conditional_Entry_Call_Statement:
          case An_Asynchronous_Select_Statement:
             {
               SgUntypedStatement* untypedStatement = NULL;

            // Path_List              Statement_Paths;
               processPathList(statement.Statement_Paths);

               switch (statementKind)
                  {
                    case An_If_Statement:
                       {
                         untypedStatement = new SgUntypedIfStatement();
                         break;
                       }

                    case A_Selective_Accept_Statement:
                       {
                         untypedStatement = new SgUntypedSelectiveAcceptStatement();
                         break;
                       }

                    case A_Timed_Entry_Call_Statement:
                       {
                         untypedStatement = new SgUntypedTimedEntryCallStatement();
                         break;
                       }

                    case A_Conditional_Entry_Call_Statement:
                       {
                         untypedStatement = new SgUntypedConditionalEntryCallStatement();
                         break;
                       }

                    case An_Asynchronous_Select_Statement:
                       {
                         untypedStatement = new SgUntypedAsynchronousSelectStatement();
                         break;
                       }

                    default:
                       {
                         printf ("default reached in processStatement(): collected cases: \n");
                         ROSE_ASSERT(false);
                       }
                  }

               ROSE_ASSERT(untypedStatement != NULL);

            // ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               if (untypedNodeMap.find(element_id) == untypedNodeMap.end())
                  {
                    processUntypedNode(untypedStatement,element_id);
                  }
                 else
                  {
                    printf ("Skipping redundent processing of element_id = %d \n",element_id);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }


#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Case_Statement,                    // 5.4
          case A_Case_Statement:
             {
            // Path_List              Statement_Paths;
            // Expression_ID          Case_Expression;
               processPathList(statement.Statement_Paths);
               printf ("Case_Expression = %d \n",statement.Case_Expression);
#if 1
               printf ("Exiting as a test! \n");
               // ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Loop_Statement,                    // 5.5
       // A_While_Loop_Statement,              // 5.5
       // A_For_Loop_Statement,                // 5.5
       // A_Block_Statement,                   // 5.6
          case A_Loop_Statement:
          case A_While_Loop_Statement:
          case A_For_Loop_Statement:
          case A_Block_Statement:
             {
            // Defining_Name_ID Statement_Identifier;
               printf ("Statement_Identifier = %d \n",statement.Statement_Identifier);

               SgUntypedStatement* untypedStatement = NULL;

               switch (statementKind)
                  {
                    case A_Loop_Statement:
                       {
                         untypedStatement = new SgUntypedLoopStatement();
                         break;
                       }

                    case A_While_Loop_Statement:
                       {
                         untypedStatement = new SgUntypedWhileStatement();
                         break;
                       }

                    case A_For_Loop_Statement:
                       {
                         untypedStatement = new SgUntypedForStatement();
                         break;
                       }

                    case A_Block_Statement:
                       {
                         untypedStatement = new SgUntypedBlockStatement();
                         break;
                       }

                    default:
                       {
                         printf ("default reached in processStatement(): collected cases: \n");
                         ROSE_ASSERT(false);
                       }
                  }

               ROSE_ASSERT(untypedStatement != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               if (untypedNodeMap.find(element_id) == untypedNodeMap.end())
                  {
                    processUntypedNode(untypedStatement,element_id);
                  }
                 else
                  {
                    printf ("Skipping redundent processing of element_id = %d \n",element_id);
#if 1
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Exit_Statement: // \todo
          case A_Null_Statement:
             {
            // Nothing to do for this case.
               printf ("Nothing to do for this case \n");

            // Now build the untyped-AST.
            // Using: SgUntypedNamedStatement (std::string statement_name);

               printf ("Building untyped AST node using element_id = %d \n",element_id);

            // Note clear how we want to support different language constructs.
            // We also might want a set of AST Build API functions to support this.
               SgUntypedNullStatement* untypedStatement = new SgUntypedNullStatement();

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedStatement;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedStatement,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Return_Statement,                  // 6.5
          case A_Return_Statement:
             {
               printf ("Return_Expression = %d \n",statement.Return_Expression);

               SgUntypedReturnStatement* untypedStatement = new SgUntypedReturnStatement();

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedStatement;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedStatement,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Procedure_Call_Statement,          // 6.4
       // An_Entry_Call_Statement,             // 9.5.3
          case An_Entry_Call_Statement:
          case A_Procedure_Call_Statement:
             {
            // Expression_ID          Called_Name;
            // Declaration_ID         Corresponding_Called_Entity;
            // Association_List       Call_Statement_Parameters;
               printf ("Called_Name                 = %d \n",statement.Called_Name);
               printf ("Corresponding_Called_Entity = %d \n",statement.Corresponding_Called_Entity);
               printf ("Call_Statement_Parameters: \n");
               processAssociationList(statement.Call_Statement_Parameters);

               SgUntypedStatement* untypedStatement = NULL;
               if (statementKind == A_Procedure_Call_Statement)
                  {
                    untypedStatement = new SgUntypedProcedureCallStatement();
                  }
                 else
                  {
                    ROSE_ASSERT(statementKind == An_Entry_Call_Statement);
                    untypedStatement = new SgUntypedEntryCallStatement();
                  }

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedStatement;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedStatement,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Raise_Statement,                   // 11.3
          case A_Raise_Statement:
             {
            // Expression_ID          Raised_Exception;
            // Expression_ID          Associated_Message;
               printf ("Raised_Exception   = %d \n",statement.Raised_Exception);
               printf ("Associated_Message = %d \n",statement.Associated_Message);

               SgUntypedRaiseStatement* raiseStatement = new SgUntypedRaiseStatement();
               processUntypedNode (raiseStatement,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Accept_Statement,                 // 9.5.2
          case An_Accept_Statement:
             {
            // bool                   Is_Name_Repeated;

            // Expression_ID          Accept_Entry_Index;
            // Name_ID                Accept_Entry_Direct_Name;
            // Parameter_Specification_List Accept_Parameters;
            // Statement_List         Accept_Body_Statements;
            // Statement_List         Accept_Body_Exception_Handlers;
            // Declaration_ID         Corresponding_Entry;

               printf ("Is_Name_Repeated = %s \n",statement.Is_Name_Repeated ? "true" : "false");
               printf ("Accept_Entry_Index = %d \n",statement.Accept_Entry_Index);
               printf ("Accept_Entry_Direct_Name = %d \n",statement.Accept_Entry_Direct_Name);
               printf ("Accept_Parameters: \n");
               processStatementList(statement.Accept_Parameters);
               printf ("Accept_Body_Statements: \n");
               processStatementList(statement.Accept_Body_Statements);
               printf ("Accept_Body_Exception_Handlers: \n");
               processStatementList(statement.Accept_Body_Exception_Handlers);
               printf ("Corresponding_Entry = %d \n",statement.Corresponding_Entry);

               SgUntypedAcceptStatement* acceptStatement = new SgUntypedAcceptStatement();
               processUntypedNode (acceptStatement,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
          case x:
             {
               printf ("x = %d \n",statement.x);
               printf ("x = %d \n",statement.x);
               break;
             }
#endif
          default:
             {
               printf ("In processStatement(): default reached in switch \n");
               ROSE_ASSERT(false);
             }
        }


#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processDefiningName( Defining_Name_Struct & definingName, int element_id)
   {
#if 0
struct Defining_Name_Struct {
  enum Defining_Name_Kinds  Defining_Name_Kind;
  char                     *Defining_Name_Image;

  // These fields are only valid for the kinds above them:
  // A_Defining_Character_Literal
  // A_Defining_Enumeration_Literal
  char                     *Position_Number_Image;
  char                     *Representation_Value_Image;
  // A_Defining_Expanded_Name
  Name_ID                   Defining_Prefix;
  Defining_Name_ID          Defining_Selector;
  // When this is the name of a constant or a deferred constant:
  Declaration_ID            Corresponding_Constant_Declaration;
  // A_Defining_Operator_Symbol:
  enum Operator_Kinds       Operator_Kind;
  Defining_Name_ID      Corresponding_Generic_Element;
  };
#endif

     Defining_Name_Kinds definingNameKind = definingName.Defining_Name_Kind;

     string definingNameImage = definingName.Defining_Name_Image;

     printf ("   In processDefiningName(): \n");
     printf ("      definingNameKind (value)   = %d \n",definingNameKind);
     printf ("      definingNameKind (name)    = %s \n",definingNameKindName(definingNameKind).c_str());

     printf ("      Defining_Name_Image        = %s \n",definingName.Defining_Name_Image);
     printf ("      Representation_Value_Image = %s \n",definingName.Representation_Value_Image);

     printf ("      Defining_Prefix            = %d \n",definingName.Defining_Prefix);
     printf ("      Defining_Selector          = %d \n",definingName.Defining_Selector);

     printf ("      Corresponding_Constant_Declaration = %d \n",definingName.Corresponding_Constant_Declaration);

     printf ("      Operator_Kind                      = %s \n",operatorKindName(definingName.Operator_Kind).c_str());

  // Build an untyped AST node.
     printf ("Building untyped AST node using element_id = %d \n",element_id);

  // Moved from Declaration_Struct
     printf ("      Corresponding_Generic_Element = %d \n",definingName.Corresponding_Generic_Element);

  // Note clear how we want to support different language constructs.
  // We also might want a set of AST Build API functions to support this.
  // SgUntypedName (std::string name)

  // DQ (9/18/2017): I want to use SgUntypedName, but this is only available in a later version of ROSE (modified by Craig).
  // SgUntypedName* untypedName = new SgUntypedName(definingNameImage);
  // SgUntypedName* untypedName = NULL; // new SgUntypedName(definingNameImage);
  // SgUntypedType *type = NULL;
  // SgUntypedInitializedName* untypedName = new SgUntypedInitializedName(type,definingNameImage);
     SgUntypedName* untypedName = new SgUntypedName(definingNameImage);

  // Put into map using Ada Node ID's as the keys.
     ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
     untypedNodeMap[element_id] = untypedName;
     ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

     printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

  // Build the attribute (element id onto the SgUntypedNode).
     setAsisAttribute (untypedName,element_id);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }




void
  Ada_ROSE_Translation::processDeclaration( Declaration_Struct & declaration, int element_id)
   {
#if 0
  // Documentation for Expression_Struct.
  enum Declaration_Kinds   Declaration_Kind;
  enum Declaration_Origins Declaration_Origin;

  // These fields are only valid for the kinds above them:
  // A_Parameter_Specification |
  // A_Formal_Object_Declaration:
  enum Mode_Kinds          Mode_Kind;
  // A_Formal_Function_Declaration |
  // A_Formal_Procedure_Declaration:
  enum Subprogram_Default_Kinds
                           Default_Kind;
  // A_Private_Type_Declaration |
  // A_Private_Extension_Declaration |
  // A_Variable_Declaration |
  // A_Constant_Declaration |
  // A_Deferred_Constant_Declaration |
  // A_Discriminant_Specification |
  // A_Loop_Parameter_Specification |
  // A_Generalized_Iterator_Specification |
  // An_Element_Iterator_Specification |
  // A_Procedure_Declaration |
  // A_Function_Declaration:
  enum Trait_Kinds         Trait_Kind;

  // TODO: add remaining valid kinds comments:
  // (all kinds)
  Defining_Name_List             Names;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
  //  A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  //  A_Formal_Type_Declaration,                // 12.5(2)
  Definition_ID                  Discriminant_Part;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  //  A_Formal_Type_Declaration,                // 12.5(2)
  Definition_ID                  Type_Declaration_View;
  //  A_Variable_Declaration,                   // 3.3.1(2)
  //  A_Constant_Declaration,                   // 3.3.1(4)
  //  A_Deferred_Constant_Declaration,          // 3.3.1(6),7.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  //  A_Discriminant_Specification,             // 3.7(5)
  //  A_Component_Declaration,                  // 3.8(6)
  //  A_Parameter_Specification,                // 6.1(15)
  //  A_Return_Variable_Specification,          // 6.5
  //  An_Object_Renaming_Declaration,           // 8.5.1(2)
  //  A_Formal_Object_Declaration,              // 12.4(2)
  Definition_ID                  Object_Declaration_View;
  // (all kinds)
  Element_List                   Aspect_Specifications;
  //  A_Variable_Declaration,                   // 3.3.1(2)
  //  A_Constant_Declaration,                   // 3.3.1(4)
  //  An_Integer_Number_Declaration,            // 3.3.2(2)
  //  A_Real_Number_Declaration,                // 3.5.6(2)
  //  A_Discriminant_Specification,             // 3.7(5)
  //  A_Component_Declaration,                  // 3.8(6)
  //  A_Parameter_Specification,                // 6.1(15)
  //  A_Return_Variable_Specification,          // 6.5
  //  A_Formal_Object_Declaration,              // 12.4(2)
  Expression_ID                  Initialization_Expression;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
  //  A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  Declaration_ID                 Corresponding_Type_Declaration;
  //  An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
  //  A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  Declaration_ID                 Corresponding_Type_Completion;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  Declaration_ID                 Corresponding_Type_Partial_View;
  //  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
  //  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
  //  A_Subtype_Declaration,                    // 3.2.2(2)
  //  A_Formal_Type_Declaration,                // 12.5(2)
  Declaration_ID                 Corresponding_First_Subtype;
  Declaration_ID                 Corresponding_Last_Constraint;
  Declaration_ID                 Corresponding_Last_Subtype;
  // (all)
  Representation_Clause_List     Corresponding_Representation_Clauses;
  //  A_Loop_Parameter_Specification,           // 5.5(4)
  //  An_Entry_Index_Specification,             // 9.5.2(2)
  Discrete_Subtype_Definition_ID Specification_Subtype_Definition;
  //  A_Generalized_Iterator_Specification,     // 5.5.2
  //  An_Element_Iterator_Specification,        // 5.5.2
  Element_ID                     Iteration_Scheme_Name;
  //  An_Element_Iterator_Specification,        // 5.5.2
  Element_ID                     Subtype_Indication;
  //  A_Procedure_Declaration,                  // 6.1(4)
  //  A_Function_Declaration,                   // 6.1(4)
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Null_Procedure_Declaration,             // 6.7
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  An_Entry_Declaration,                     // 9.5.2(2)
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Formal_Procedure_Declaration,           // 12.6(2)
  //  A_Formal_Function_Declaration,            // 12.6(2)
  Parameter_Specification_List   Parameter_Profile;
  //  A_Function_Declaration,                   // 6.1(4)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Formal_Function_Declaration,            // 12.6(2)
  Element_ID                     Result_Profile;
  //  An_Expression_Function_Declaration,       // 6.8
  Expression_ID                  Result_Expression;
  //  A_Procedure_Declaration,                  // 6.1(4)
  //  A_Function_Declaration,                   // 6.1(4)
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Null_Procedure_Declaration,             // 6.7
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  An_Entry_Declaration,                     // 9.5.2(2)
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Formal_Procedure_Declaration,           // 12.6(2)
  //  A_Formal_Function_Declaration,            // 12.6(2)
  bool                           Is_Overriding_Declaration;
  bool                           Is_Not_Overriding_Declaration;
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Package_Body_Declaration,               // 7.2(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  Element_List                   Body_Declarative_Items;
  Statement_List                 Body_Statements;
  Exception_Handler_List         Body_Exception_Handlers;
  Declaration_ID                 Body_Block_Statement;
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Package_Declaration,                    // 7.1(2)
  //  A_Package_Body_Declaration               // 7.2(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  A_Protected_Body_Declaration,             // 9.4(7)
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  bool                           Is_Name_Repeated;
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  //  A_Procedure_Declaration,                  // 6.1(4)
  //  A_Function_Declaration,                   // 6.1(4)
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Null_Procedure_Declaration,             // 6.7
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Package_Declaration,                    // 7.1(2)
  //  A_Package_Body_Declaration,               // 7.2(2)
  //  A_Package_Renaming_Declaration,           // 8.5.3(2)
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  A_Generic_Package_Renaming_Declaration,   // 8.5.5(2)
  //  A_Generic_Procedure_Renaming_Declaration, // 8.5.5(2)
  //  A_Generic_Function_Renaming_Declaration,  // 8.5.5(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  A_Protected_Body_Declaration,             // 9.4(7)
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  //  An_Entry_Index_Specification,             // 9.5.2(2)
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Package_Body_Stub,                      // 10.1.3(4)
  //  A_Task_Body_Stub,                         // 10.1.3(5)
  //  A_Protected_Body_Stub,                    // 10.1.3(6)
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  //  A_Package_Instantiation,                  // 12.3(2)
  //  A_Procedure_Instantiation,                // 12.3(2)
  //  A_Function_Instantiation,                 // 12.3(2)
  //  A_Formal_Package_Declaration,             // 12.7(2)
  //  A_Formal_Package_Declaration_With_Box     // 12.7(3)
  Declaration_ID                 Corresponding_Declaration;
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  //  A_Procedure_Declaration,                  // 6.1(4)
  //  A_Function_Declaration,                   // 6.1(4)
  //  An_Entry_Declaration,                     // 9.5.2(2)
  //  An_Entry_Index_Specification,             // 9.5.2(2)
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  //  A_Package_Instantiation,                  // 12.3(2)
  //  A_Procedure_Instantiation,                // 12.3(2)
  //  A_Function_Instantiation,                 // 12.3(2)
  //  A_Formal_Package_Declaration,             // 12.7(2)
  Declaration_ID                 Corresponding_Body;
  //  A_Procedure_Declaration,                  // 6.1(4)
  //  A_Function_Declaration,                   // 6.1(4)
  Declaration_ID                 Corresponding_Subprogram_Derivation;
  //  A_Procedure_Declaration,                  // 6.1(4)
  //  A_Function_Declaration,                   // 6.1(4)
  //  An_Expression_Function_Declaration,       // 6.8
  Type_Definition_ID             Corresponding_Type;
  //  A_Function_Declaration,                   // 6.1(4)
  Declaration_ID                 Corresponding_Equality_Operator;
  //  A_Package_Declaration,                    // 7.1(2)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  Declarative_Item_List          Visible_Part_Declarative_Items;
  bool                           Is_Private_Present;
  Declarative_Item_List          Private_Part_Declarative_Items;
  //  A_Task_Type_Declaration,                  // 9.1(2)
  //  A_Protected_Type_Declaration,             // 9.4(2)
  //  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  //  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
  Expression_List                Declaration_Interface_List;
  //  An_Object_Renaming_Declaration,           // 8.5.1(2)
  //  An_Exception_Renaming_Declaration,        // 8.5.2(2)
  //  A_Package_Renaming_Declaration,           // 8.5.3(2)
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  A_Generic_Package_Renaming_Declaration,   // 8.5.5(2)
  //  A_Generic_Procedure_Renaming_Declaration, // 8.5.5(2)
  //  A_Generic_Function_Renaming_Declaration,  // 8.5.5(2)
  Expression_ID                  Renamed_Entity;
  Expression_ID                  Corresponding_Base_Entity;
  //  A_Protected_Body_Declaration,             // 9.4(7)
  Declaration_List               Protected_Operation_Items;
  //  An_Entry_Declaration,                     // 9.5.2(2)
  Discrete_Subtype_Definition_ID Entry_Family_Definition;
  //  An_Entry_Body_Declaration,                // 9.5.2(5)
  Declaration_ID                 Entry_Index_Specification;
  Expression_ID                  Entry_Barrier;
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  //  A_Package_Body_Stub,                      // 10.1.3(4)
  //  A_Task_Body_Stub,                         // 10.1.3(5)
  //  A_Protected_Body_Stub,                    // 10.1.3(6)
  Declaration_ID                 Corresponding_Subunit;
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Package_Body_Declaration,               // 7.2(2)
  //  A_Task_Body_Declaration,                  // 9.1(6)
  //  A_Protected_Body_Declaration,             // 9.4(7)
  bool                           Is_Subunit;
  Declaration_ID                 Corresponding_Body_Stub;
  //  A_Generic_Procedure_Declaration,          // 12.1(2)
  //  A_Generic_Function_Declaration,           // 12.1(2)
  //  A_Generic_Package_Declaration,            // 12.1(2)
  Element_List                   Generic_Formal_Part;
  //  A_Package_Instantiation,                  // 12.3(2)
  //  A_Procedure_Instantiation,                // 12.3(2)
  //  A_Function_Instantiation,                 // 12.3(2)
  //  A_Formal_Package_Declaration,             // 12.7(2)
  //  A_Formal_Package_Declaration_With_Box     // 12.7(3)
  Expression_ID                  Generic_Unit_Name;
  Association_List               Generic_Actual_Part;
  //  A_Formal_Procedure_Declaration,           // 12.6(2)
  //  A_Formal_Function_Declaration,            // 12.6(2)
  Expression_ID                  Formal_Subprogram_Default;
  //  A_Procedure_Declaration,                  // 6.1(4)
  //  A_Function_Declaration,                   // 6.1(4)
  //  A_Procedure_Body_Declaration,             // 6.3(2)
  //  A_Function_Body_Declaration,              // 6.3(2)
  //  A_Null_Procedure_Declaration,             // 6.7
  //  An_Expression_Function_Declaration,       // 6.8
  //  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  //  A_Function_Renaming_Declaration,          // 8.5.4(2)
  //  A_Procedure_Body_Stub,                    // 10.1.3(3)
  //  A_Function_Body_Stub,                     // 10.1.3(3)
  bool                           Is_Dispatching_Operation;
#endif

#define DEBUG_UNTYPED_NODE_GENERATION 1

     Declaration_Kinds        declarationKind = declaration.Declaration_Kind;
     Subprogram_Default_Kinds defaultKind     = declaration.Default_Kind;
  // Trait_Kinds              traitKind       = declaration.Trait_Kind;
     Defining_Name_List &     names           = declaration.Names;

     Declaration_Origins declarationOrigin    = declaration.Declaration_Origin;

  // Note that ROSE defines statements and declarations a bit differently.
  // E.g. a block declaration here is a SgScopeStatement in ROSE (specicially a SgBasicBlock).

     printf ("   In processDeclaration(): element_id = %d \n",element_id);
     printf ("      declarationKind (value) = %d \n",declarationKind);
     printf ("      declarationKind (name)  = %s \n",declarationKindName(declarationKind).c_str());

     printf ("      defaultKind (value) = %d \n",defaultKind);
     printf ("      defaultKind (name)  = %s \n",subprogramDefaultKindName(defaultKind).c_str());

  // printf ("      traitKind (value) = %d \n",traitKind);
  // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

     printf ("      declarationOrigin (value) = %d \n",declarationOrigin);
     printf ("      declarationOrigin (name)  = %s \n",declarationOriginName(declarationOrigin).c_str());

  // Defining_Name_List             Names;
     printf ("      Names: \n");
     processDefiningNameList(names);

  // Element_List                   Aspect_Specifications;
  // Element_List & aspectSpecifications = declaration.Aspect_Specifications;
     Element_ID_List & aspectSpecifications = declaration.Aspect_Specifications;
     processElementList(aspectSpecifications);

     Representation_Clause_List & correspondingRepresentationClauses = declaration.Corresponding_Representation_Clauses;
     processRepresentationClauseList(correspondingRepresentationClauses);

     switch (declarationKind)
        {
       // A_Parameter_Specification |
       // A_Formal_Object_Declaration:
          case A_Parameter_Specification:
          case A_Formal_Object_Declaration:
             {
               Mode_Kinds modeKind                   = declaration.Mode_Kind;
               printf ("      modeKind (value) = %d \n",modeKind);
               printf ("      modeKind (name)  = %s \n",modeKindName(modeKind).c_str());

               Definition_ID objectDeclarationView = declaration.Object_Declaration_View;
               printf ("objectDeclarationView = %d \n",objectDeclarationView);

               Expression_ID initializationExpression = declaration.Initialization_Expression;
               printf ("initializationExpression = %d \n",initializationExpression);

               ROSE_ASSERT(names.Length > 0);
               //~ ROSE_ASSERT(names.Length == 1);
               int name_id = names.IDs[0];

               printf ("name_id = %d \n",name_id);

            // string name = names[0];
               string name = "__unknown__";

            // Lookup the name in the ASIS node given by the name_id.
               ROSE_ASSERT(asisMap.find(name_id) != asisMap.end());
               Element_Struct* element_struct = asisMap[name_id];
               ROSE_ASSERT(element_struct != NULL);

               Element_Union & element_union = element_struct->The_Union;

               ROSE_ASSERT(element_struct->Element_Kind == A_Defining_Name);

               Defining_Name_Struct & definingName = element_union.Defining_Name;

               name = definingName.Defining_Name_Image;

               printf ("name = %s \n",name.c_str());

            // Build the untyped node we will use to represent the structure of the AST.
               SgUntypedType *type = NULL;
               SgUntypedInitializedName* untypedName = new SgUntypedInitializedName(type,name);
#if 0
            // Process the objectDeclarationView.
               int objectDeclarationView_id = objectDeclarationView;

               ROSE_ASSERT(asisMap.find(objectDeclarationView) != asisMap.end());

               Element_Struct* elementStruct = asisMap[objectDeclarationView_id];

            // int mode_id =
#endif
#if 0
               switch (modeKind)
                  {
                    case Not_A_Mode:
                       {
                         s = "Not_A_Mode";
                         break;
                       }

                    case A_Default_In_Mode: s = "A_Default_In_Mode"; break;
                    case An_In_Mode:        s = "An_In_Mode";        break;
                    case An_Out_Mode:       s = "An_Out_Mode";       break;
                    case An_In_Out_Mode:    s = "An_In_Out_Mode";    break;

                    default:
                       {
                         printf ("Error: default called in switch for Ada_ROSE_Translation::modeKindName(): x = %d \n",x);
                         ROSE_ASSERT(false);
                       }
                  }
#endif

               printf ("Skipping setting the modeKind in the untyped node \n");

            // untypedName->set_mode(modeKind);

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedName;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedName,element_id);

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // A_Formal_Function_Declaration |
       // A_Formal_Procedure_Declaration:
       // case A_Formal_Function_Declaration:
       // case A_Formal_Procedure_Declaration:
             {
               Subprogram_Default_Kinds subprogramDefaultKind = declaration.Default_Kind;
               printf ("      subprogramDefaultKind (value) = %d \n",subprogramDefaultKind);
               printf ("      subprogramDefaultKind (name)  = %s \n",subprogramDefaultKindName(subprogramDefaultKind).c_str());

               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);
               break;
             }
#endif

          case A_Formal_Function_Declaration:
             {
               Subprogram_Default_Kinds subprogramDefaultKind = declaration.Default_Kind;
               printf ("      subprogramDefaultKind (value) = %d \n",subprogramDefaultKind);
               printf ("      subprogramDefaultKind (name)  = %s \n",subprogramDefaultKindName(subprogramDefaultKind).c_str());

               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               Element_ID resultProfile = declaration.Result_Profile;
               printf ("      resultProfile = %d \n",resultProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Expression_ID formalSubprogramDefault = declaration.Formal_Subprogram_Default;
               printf ("formalSubprogramDefault = %d \n",formalSubprogramDefault);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }


       // A_Private_Type_Declaration |
       // A_Private_Extension_Declaration |
       // A_Variable_Declaration |
       // A_Constant_Declaration |
       // A_Deferred_Constant_Declaration |
       // A_Discriminant_Specification |
       // A_Loop_Parameter_Specification |
       // A_Generalized_Iterator_Specification |
       // An_Element_Iterator_Specification |
       // A_Procedure_Declaration |
       // A_Function_Declaration:
       // case A_Variable_Declaration:
       // case A_Constant_Declaration:
       // case A_Deferred_Constant_Declaration:
       // case A_Discriminant_Specification:
       // case A_Loop_Parameter_Specification:
       // case A_Generalized_Iterator_Specification:
          case An_Element_Iterator_Specification:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("      traitKind (value) = %d \n",traitKind);
            // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Discrete_Subtype_Definition_ID specificationSubtypeDefinition = declaration.Specification_Subtype_Definition;
               printf ("specificationSubtypeDefinition  = %d \n",specificationSubtypeDefinition);

               Element_ID iterationSchemeName = declaration.Iteration_Scheme_Name;
               printf ("iterationSchemeName  = %d \n",iterationSchemeName);

               Element_ID subtypeIndication = declaration.Subtype_Indication;
               printf ("subtypeIndication  = %d \n",subtypeIndication);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Loop_Parameter_Specification:
       // case A_Generalized_Iterator_Specification:
       // case An_Element_Iterator_Specification:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("      traitKind (value) = %d \n",traitKind);
            // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Discrete_Subtype_Definition_ID specificationSubtypeDefinition = declaration.Specification_Subtype_Definition;
               printf ("specificationSubtypeDefinition  = %d \n",specificationSubtypeDefinition);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               //~ ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Discriminant_Specification:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("      traitKind (value) = %d \n",traitKind);
            // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Definition_ID objectDeclarationView = declaration.Object_Declaration_View;
               printf ("objectDeclarationView = %d \n",objectDeclarationView);

               Expression_ID initializationExpression = declaration.Initialization_Expression;
               printf ("initializationExpression = %d \n",initializationExpression);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Variable_Declaration:
          case A_Constant_Declaration:
       // case A_Deferred_Constant_Declaration:
       // case A_Discriminant_Specification:
       // case A_Loop_Parameter_Specification:
       // case A_Generalized_Iterator_Specification:
       // case An_Element_Iterator_Specification:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("      traitKind (value) = %d \n",traitKind);
            // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Definition_ID objectDeclarationView = declaration.Object_Declaration_View;
               printf ("objectDeclarationView = %d \n",objectDeclarationView);

               Expression_ID initializationExpression = declaration.Initialization_Expression;
               printf ("initializationExpression = %d \n",initializationExpression);

               //~ SgUntypedType* type = NULL;
               SgUntypedVariableDeclaration* untypedVariableDeclaration = new SgUntypedVariableDeclaration();
               ROSE_ASSERT(untypedVariableDeclaration != NULL);

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedVariableDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedVariableDeclaration,element_id);

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // case A_Variable_Declaration:
       // case A_Constant_Declaration:
          case A_Deferred_Constant_Declaration:
       // case A_Discriminant_Specification:
       // case A_Loop_Parameter_Specification:
       // case A_Generalized_Iterator_Specification:
       // case An_Element_Iterator_Specification:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("      traitKind (value) = %d \n",traitKind);
            // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Definition_ID objectDeclarationView = declaration.Object_Declaration_View;
               printf ("objectDeclarationView = %d \n",objectDeclarationView);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Private_Type_Declaration:
          case A_Private_Extension_Declaration:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("     traitKind (value) = %d \n",traitKind);
            // printf ("     traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Definition_ID discriminantPart = declaration.Discriminant_Part;
               printf ("     discriminantPart = %d \n",discriminantPart);

               Definition_ID typeDeclarationView = declaration.Type_Declaration_View;
               printf ("     typeDeclarationView = %d \n",typeDeclarationView);

               Declaration_ID correspondingTypeDeclaration = declaration.Corresponding_Type_Declaration;
               printf ("     correspondingTypeDeclaration = %d \n",correspondingTypeDeclaration);

               Declaration_ID correspondingTypeCompletion = declaration.Corresponding_Type_Completion;
               printf ("     correspondingTypeCompletion = %d \n",correspondingTypeCompletion);

               Declaration_ID correspondingTypePartialView = declaration.Corresponding_Type_Partial_View;
               printf ("     correspondingTypePartialView = %d \n",correspondingTypePartialView);

               Declaration_ID correspondingFirstSubtype   = declaration.Corresponding_First_Subtype;
               Declaration_ID correspondingLastConstraint = declaration.Corresponding_Last_Constraint;
               Declaration_ID correspondingLastSubtype    = declaration.Corresponding_Last_Subtype;
               printf ("     correspondingFirstSubtype   = %d \n",correspondingFirstSubtype);
               printf ("     correspondingLastConstraint = %d \n",correspondingLastConstraint);
               printf ("     correspondingLastSubtype    = %d \n",correspondingLastSubtype);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Ordinary_Type_Declaration,            // 3.2.1(3)
       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
       // An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
       // A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
       // A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
       // A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
       // A_Formal_Type_Declaration,                // 12.5(2)
       // case An_Ordinary_Type_Declaration:
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
          case An_Incomplete_Type_Declaration:
          case A_Tagged_Incomplete_Type_Declaration:
       // case A_Formal_Type_Declaration:
             {
               Definition_ID discriminantPart = declaration.Discriminant_Part;
               printf ("discriminantPart = %d \n",discriminantPart);

               Declaration_ID correspondingTypeDeclaration = declaration.Corresponding_Type_Declaration;
               printf ("correspondingTypeDeclaration = %d \n",correspondingTypeDeclaration);

               Declaration_ID correspondingTypeCompletion = declaration.Corresponding_Type_Completion;
               printf ("correspondingTypeCompletion = %d \n",correspondingTypeCompletion);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Ordinary_Type_Declaration,            // 3.2.1(3)
       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
          case An_Ordinary_Type_Declaration:
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
             {
               Definition_ID discriminantPart = declaration.Discriminant_Part;
               printf ("discriminantPart = %d \n",discriminantPart);

               Definition_ID typeDeclarationView = declaration.Type_Declaration_View;
               printf ("typeDeclarationView = %d \n",typeDeclarationView);

               Declaration_ID correspondingTypeDeclaration = declaration.Corresponding_Type_Declaration;
               printf ("correspondingTypeDeclaration = %d \n",correspondingTypeDeclaration);

               Declaration_ID correspondingTypePartialView = declaration.Corresponding_Type_Partial_View;
               printf ("correspondingTypePartialView = %d \n",correspondingTypePartialView);

               Declaration_ID correspondingFirstSubtype   = declaration.Corresponding_First_Subtype;
               Declaration_ID correspondingLastConstraint = declaration.Corresponding_Last_Constraint;
               Declaration_ID correspondingLastSubtype    = declaration.Corresponding_Last_Subtype;
               printf ("correspondingFirstSubtype   = %d \n",correspondingFirstSubtype);
               printf ("correspondingLastConstraint = %d \n",correspondingLastConstraint);
               printf ("correspondingLastSubtype    = %d \n",correspondingLastSubtype);
#if 0
               SgUntypedType* type = NULL;
               SgUntypedVariableDeclaration* untypedVariableDeclaration = new SgUntypedVariableDeclaration(type);
               ROSE_ASSERT(untypedVariableDeclaration != NULL);

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedVariableDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedVariableDeclaration,element_id);
#else
            // We can't know the name at this point since it is or will be held as a SgUntypedName.
               SgUntypedStructureDeclaration* untypedStructureDeclaration = new SgUntypedStructureDeclaration();
               ROSE_ASSERT(untypedStructureDeclaration != NULL);

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedStructureDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedStructureDeclaration,element_id);
#endif

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Task_Type_Declaration:
          case A_Protected_Type_Declaration:
             {
               Definition_ID discriminantPart = declaration.Discriminant_Part;
               printf ("discriminantPart = %d \n",discriminantPart);

               Definition_ID typeDeclarationView = declaration.Type_Declaration_View;
               printf ("typeDeclarationView = %d \n",typeDeclarationView);

               Declaration_ID correspondingTypeDeclaration = declaration.Corresponding_Type_Declaration;
               printf ("correspondingTypeDeclaration = %d \n",correspondingTypeDeclaration);

               Declaration_ID correspondingTypePartialView = declaration.Corresponding_Type_Partial_View;
               printf ("correspondingTypePartialView = %d \n",correspondingTypePartialView);

               Declaration_ID correspondingFirstSubtype   = declaration.Corresponding_First_Subtype;
               Declaration_ID correspondingLastConstraint = declaration.Corresponding_Last_Constraint;
               Declaration_ID correspondingLastSubtype    = declaration.Corresponding_Last_Subtype;
               printf ("correspondingFirstSubtype   = %d \n",correspondingFirstSubtype);
               printf ("correspondingLastConstraint = %d \n",correspondingLastConstraint);
               printf ("correspondingLastSubtype    = %d \n",correspondingLastSubtype);

               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Expression_List & declarationInterfaceList = declaration.Declaration_Interface_List;
               printf ("declarationInterfaceList \n");
               processExpressionList(declarationInterfaceList);

            // Get the name from the name list.
               ROSE_ASSERT(names.Length == 1);
               int taskName_id = names.IDs[0];

               printf ("taskName_id = %d \n",taskName_id);

               ROSE_ASSERT(untypedNodeMap.find(taskName_id) != untypedNodeMap.end());
               SgUntypedNode* untypedNode = untypedNodeMap[taskName_id];
               ROSE_ASSERT(untypedNode != NULL);
            // SgUntypedInitializedName* untypedInitializedName = isSgUntypedInitializedName(untypedNode);
            // ROSE_ASSERT(untypedInitializedName != NULL);
            // string packageName = untypedInitializedName->get_name();
               SgUntypedName* untypedName = isSgUntypedName(untypedNode);
               ROSE_ASSERT(untypedName != NULL);
               string taskName = untypedName->get_name();

               printf ("taskName = %s \n",taskName.c_str());

               SgUntypedTaskDeclaration* untypedTaskDeclaration = new SgUntypedTaskDeclaration(taskName);
               ROSE_ASSERT(untypedTaskDeclaration != NULL);

               SgUntypedScope* untypedScope = new SgUntypedScope();
               ROSE_ASSERT(untypedScope != NULL);

               untypedTaskDeclaration->set_scope(untypedScope);

               SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
               ROSE_ASSERT(untypedStatementList != NULL);

            // Connect IR nodes.
               untypedScope->set_statement_list(untypedStatementList);

            // Note that we can't connect the statements in the function body, until a second pass over the untyped AST.

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedTaskDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedTaskDeclaration,element_id);

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
       // A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
       // A_Formal_Type_Declaration,                // 12.5(2)
       // case A_Private_Type_Declaration:
       // case A_Private_Extension_Declaration:
          case A_Formal_Type_Declaration:
             {
               Definition_ID discriminantPart = declaration.Discriminant_Part;
               printf ("discriminantPart = %d \n",discriminantPart);

               Definition_ID typeDeclarationView = declaration.Type_Declaration_View;
               printf ("typeDeclarationView = %d \n",typeDeclarationView);

               Declaration_ID correspondingFirstSubtype   = declaration.Corresponding_First_Subtype;
               Declaration_ID correspondingLastConstraint = declaration.Corresponding_Last_Constraint;
               Declaration_ID correspondingLastSubtype    = declaration.Corresponding_Last_Subtype;
               printf ("correspondingFirstSubtype   = %d \n",correspondingFirstSubtype);
               printf ("correspondingLastConstraint = %d \n",correspondingLastConstraint);
               printf ("correspondingLastSubtype    = %d \n",correspondingLastSubtype);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Variable_Declaration,                   // 3.3.1(2)
       // A_Constant_Declaration,                   // 3.3.1(4)
       // A_Deferred_Constant_Declaration,          // 3.3.1(6),7.4(2)
       // A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
       // A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
       // A_Discriminant_Specification,             // 3.7(5)
       // A_Component_Declaration,                  // 3.8(6)
       // A_Parameter_Specification,                // 6.1(15)
       // A_Return_Variable_Specification,          // 6.5
       // An_Object_Renaming_Declaration,           // 8.5.1(2)
       // A_Formal_Object_Declaration,              // 12.4(2)
       // case A_Variable_Declaration:
       // case A_Constant_Declaration:
       // case A_Deferred_Constant_Declaration:
       // case A_Single_Task_Declaration:
       // case A_Single_Protected_Declaration:
       // case A_Discriminant_Specification:
       // case A_Component_Declaration:
       // case A_Parameter_Specification:
       // case A_Return_Variable_Specification:
          case An_Object_Renaming_Declaration:
       // case A_Formal_Object_Declaration:
             {
               Definition_ID objectDeclarationView = declaration.Object_Declaration_View;
               printf ("objectDeclarationView = %d \n",objectDeclarationView);

               Expression_ID renamedEntity           = declaration.Renamed_Entity;
               printf ("      renamedEntity = %d \n",renamedEntity);
               Expression_ID correspondingBaseEntity = declaration.Corresponding_Base_Entity;
               printf ("      correspondingBaseEntity = %d \n",correspondingBaseEntity);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Single_Task_Declaration:
          case A_Single_Protected_Declaration:
             {
               Definition_ID objectDeclarationView = declaration.Object_Declaration_View;
               printf ("objectDeclarationView = %d \n",objectDeclarationView);

               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Expression_List & declarationInterfaceList = declaration.Declaration_Interface_List;
               printf ("declarationInterfaceList \n");
               processExpressionList(declarationInterfaceList);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }


       // case A_Single_Task_Declaration:
       // case A_Single_Protected_Declaration:
       // case A_Discriminant_Specification:
          case A_Component_Declaration:
       // case A_Parameter_Specification:
          case A_Return_Variable_Specification:
       // case An_Object_Renaming_Declaration:
       // case A_Formal_Object_Declaration:
             {
               Definition_ID objectDeclarationView = declaration.Object_Declaration_View;
               printf ("objectDeclarationView = %d \n",objectDeclarationView);

               Expression_ID initializationExpression = declaration.Initialization_Expression;
               printf ("initializationExpression = %d \n",initializationExpression);


               if (declarationKind == A_Component_Declaration)
                  {
                    //~ SgUntypedType* type = NULL;
                    SgUntypedVariableDeclaration* untypedVariableDeclaration = new SgUntypedVariableDeclaration();
                    ROSE_ASSERT(untypedVariableDeclaration != NULL);

                 // Put into map using Ada Node ID's as the keys.
                    ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
                    untypedNodeMap[element_id] = untypedVariableDeclaration;
                    ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

                    printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

                 // Build the attribute (element id onto the SgUntypedNode).
                    setAsisAttribute (untypedVariableDeclaration,element_id);
                  }
                 else
                  {
                    printf ("Case of A_Return_Variable_Specification not handled! \n");
                    ROSE_ASSERT(false);
                  }

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Variable_Declaration,                   // 3.3.1(2)
       // A_Constant_Declaration,                   // 3.3.1(4)
       // An_Integer_Number_Declaration,            // 3.3.2(2)
       // A_Real_Number_Declaration,                // 3.5.6(2)
       // A_Discriminant_Specification,             // 3.7(5)
       // A_Component_Declaration,                  // 3.8(6)
       // A_Parameter_Specification,                // 6.1(15)
       // A_Return_Variable_Specification,          // 6.5
       // A_Formal_Object_Declaration,              // 12.4(2)
       // case A_Variable_Declaration:
       // case A_Constant_Declaration:
          case An_Integer_Number_Declaration:
          case A_Real_Number_Declaration:
       // case A_Discriminant_Specification:
       // case A_Component_Declaration:
       // case A_Parameter_Specification:
       // case A_Return_Variable_Specification:
       // case A_Formal_Object_Declaration:
             {
               Expression_ID initializationExpression = declaration.Initialization_Expression;
               printf ("initializationExpression = %d \n",initializationExpression);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               //~ ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // An_Ordinary_Type_Declaration,            // 3.2.1(3)
       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
       // An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
       // A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
       // A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
       // A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
       // case An_Ordinary_Type_Declaration:
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
       // case An_Incomplete_Type_Declaration:
       // case A_Tagged_Incomplete_Type_Declaration:
       // case A_Private_Type_Declaration:
       // case A_Private_Extension_Declaration:
             {
               Declaration_ID correspondingTypeDeclaration = declaration.Corresponding_Type_Declaration;
               printf ("correspondingTypeDeclaration = %d \n",correspondingTypeDeclaration);
               break;
             }
#endif

#if 0
       // An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
       // A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
       // A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
       // A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
       // case An_Incomplete_Type_Declaration:
       // case A_Tagged_Incomplete_Type_Declaration:
       // case A_Private_Type_Declaration:
       // case A_Private_Extension_Declaration:
             {
               Declaration_ID correspondingTypeCompletion = declaration.Corresponding_Type_Completion;
               printf ("correspondingTypeCompletion = %d \n",correspondingTypeCompletion);
               break;
             }
#endif
#if 0
       // An_Ordinary_Type_Declaration,            // 3.2.1(3)
       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
       // A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
       // A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
       // case An_Ordinary_Type_Declaration:
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
       // case A_Private_Type_Declaration:
       // case A_Private_Extension_Declaration:
             {
               Declaration_ID correspondingTypePartialView = declaration.Corresponding_Type_Partial_View;
               printf ("correspondingTypePartialView = %d \n",correspondingTypePartialView);
               break;
             }
#endif

       // An_Ordinary_Type_Declaration,             // 3.2.1(3)
       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
       // A_Private_Type_Declaration,               // 3.2.1(2),7.3(2)
       // A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3)
       // A_Subtype_Declaration,                    // 3.2.2(2)
       // A_Formal_Type_Declaration,                // 12.5(2)
       // case An_Ordinary_Type_Declaration:
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
       // case A_Private_Type_Declaration:
       // case A_Private_Extension_Declaration:
          case A_Subtype_Declaration:
       // case A_Formal_Type_Declaration:
             {
            // Data members associated with this kind (from spreadsheet):
            // Trait_Kind
            // Has_Abstract
            // Has_Limited
            // Has_Private
            // Discriminant_Part
            // Type_Declaration_View
            // Corresponding_Type_Declaration
            // Corresponding_Type_Completion
            // Corresponding_Type_Partial_View
            // Corresponding_First_Subtype
            // Corresponding_Last_Constraint
            // Corresponding_Last_Subtype

               Declaration_ID correspondingFirstSubtype   = declaration.Corresponding_First_Subtype;
               Declaration_ID correspondingLastConstraint = declaration.Corresponding_Last_Constraint;
               Declaration_ID correspondingLastSubtype    = declaration.Corresponding_Last_Subtype;
               printf ("correspondingFirstSubtype   = %d \n",correspondingFirstSubtype);
               printf ("correspondingLastConstraint = %d \n",correspondingLastConstraint);
               printf ("correspondingLastSubtype    = %d \n",correspondingLastSubtype);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               //~ ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // A_Loop_Parameter_Specification,           // 5.5(4)
       // An_Entry_Index_Specification,             // 9.5.2(2)
       // case A_Loop_Parameter_Specification:
       // case An_Entry_Index_Specification:
             {
               Discrete_Subtype_Definition_ID specificationSubtypeDefinition = declaration.Specification_Subtype_Definition;
               printf ("specificationSubtypeDefinition  = %d \n",specificationSubtypeDefinition);
               break;
             }
#endif

       // A_Generalized_Iterator_Specification,     // 5.5.2
       // An_Element_Iterator_Specification,        // 5.5.2
          case A_Generalized_Iterator_Specification:
       // case An_Element_Iterator_Specification:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("      traitKind (value) = %d \n",traitKind);
            // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Element_ID iterationSchemeName = declaration.Iteration_Scheme_Name;
               printf ("iterationSchemeName  = %d \n",iterationSchemeName);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // An_Element_Iterator_Specification,        // 5.5.2
          case An_Element_Iterator_Specification:
             {
               Discrete_Subtype_Definition_ID specificationSubtypeDefinition = declaration.Specification_Subtype_Definition;
               printf ("specificationSubtypeDefinition  = %d \n",specificationSubtypeDefinition);

               Element_ID iterationSchemeName = declaration.Iteration_Scheme_Name;
               printf ("iterationSchemeName  = %d \n",iterationSchemeName);

               Element_ID subtypeIndication = declaration.Subtype_Indication;
               printf ("subtypeIndication  = %d \n",subtypeIndication);
               break;
             }
#endif

#if 0
       // A_Procedure_Declaration,                  // 6.1(4)
       // A_Function_Declaration,                   // 6.1(4)
       // A_Procedure_Body_Declaration,             // 6.3(2)
       // A_Function_Body_Declaration,              // 6.3(2)
       // A_Null_Procedure_Declaration,             // 6.7
       // An_Expression_Function_Declaration,       // 6.8
       // A_Procedure_Renaming_Declaration,         // 8.5.4(2)
       // A_Function_Renaming_Declaration,          // 8.5.4(2)
       // An_Entry_Declaration,                     // 9.5.2(2)
       // An_Entry_Body_Declaration,                // 9.5.2(5)
       // A_Procedure_Body_Stub,                    // 10.1.3(3)
       // A_Function_Body_Stub,                     // 10.1.3(3)
       // A_Generic_Procedure_Declaration,          // 12.1(2)
       // A_Generic_Function_Declaration,           // 12.1(2)
       // A_Formal_Procedure_Declaration,           // 12.6(2)
       // A_Formal_Function_Declaration,            // 12.6(2)
       // case A_Procedure_Declaration:
       // case A_Function_Declaration:
       // case A_Procedure_Body_Declaration:
       // case A_Function_Body_Declaration:
             // case A_Null_Procedure_Declaration:
       // case An_Expression_Function_Declaration:
             // case A_Procedure_Renaming_Declaration:
       // case A_Function_Renaming_Declaration:
             // case An_Entry_Declaration:
             // case An_Entry_Body_Declaration:
             // case A_Procedure_Body_Stub:
       // case A_Function_Body_Stub:
             // case A_Generic_Procedure_Declaration:
       // case A_Generic_Function_Declaration:
       // case A_Formal_Procedure_Declaration:
       // case A_Formal_Function_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);
               break;
             }
#endif

       // case A_Null_Procedure_Declaration:
       // case A_Procedure_Renaming_Declaration:
          case An_Entry_Declaration:
       // case An_Entry_Body_Declaration:
       // case A_Procedure_Body_Stub:
       // case A_Generic_Procedure_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration    = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Discrete_Subtype_Definition_ID entryFamilyDefinition = declaration.Entry_Family_Definition;
               printf ("      entryFamilyDefinition = %d \n",entryFamilyDefinition);

            // Get the name from the name list.
               ROSE_ASSERT(names.Length == 1);
               int functionName_id = names.IDs[0];

               printf ("functionName_id = %d \n",functionName_id);

               ROSE_ASSERT(untypedNodeMap.find(functionName_id) != untypedNodeMap.end());
               SgUntypedNode* untypedNode = untypedNodeMap[functionName_id];
               ROSE_ASSERT(untypedNode != NULL);
            // SgUntypedInitializedName* untypedInitializedName = isSgUntypedInitializedName(untypedNode);
            // ROSE_ASSERT(untypedInitializedName != NULL);
            // string functionName = untypedInitializedName->get_name();
               SgUntypedName* untypedName = isSgUntypedName(untypedNode);
               ROSE_ASSERT(untypedName != NULL);
               string functionName = untypedName->get_name();

               printf ("functionName = %s \n",functionName.c_str());

               SgUntypedFunctionDeclaration* untypedFunctionDeclaration = new SgUntypedFunctionDeclaration(functionName);
               ROSE_ASSERT(untypedFunctionDeclaration != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedFunctionDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedFunctionDeclaration,element_id);

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Null_Procedure_Declaration:
       // case A_Procedure_Renaming_Declaration:
       // case An_Entry_Declaration:
       // case An_Entry_Body_Declaration:
       // case A_Procedure_Body_Stub:
       // case A_Generic_Procedure_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Procedure_Body_Stub:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingSubunit = declaration.Corresponding_Subunit;
               printf ("      correspondingSubunit = %d \n",correspondingSubunit);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Procedure_Renaming_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Expression_ID renamedEntity           = declaration.Renamed_Entity;
               printf ("      renamedEntity = %d \n",renamedEntity);
               Expression_ID correspondingBaseEntity = declaration.Corresponding_Base_Entity;
               printf ("      correspondingBaseEntity = %d \n",correspondingBaseEntity);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Generic_Procedure_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Element_ID_List & genericFormalPart = declaration.Generic_Formal_Part;
               printf ("      genericFormalPart: \n");
               processElementList(genericFormalPart);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Entry_Body_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Element_ID_List        & bodyDeclarativeItems  = declaration.Body_Declarative_Items;
               printf ("      bodyDeclarativeItems: \n");
               processElementList(bodyDeclarativeItems);
               Statement_List         & bodyStatements        = declaration.Body_Statements;
               printf ("      bodyStatements: \n");
               processStatementList(bodyStatements);
               Exception_Handler_List & bodyExceptionHandlers = declaration.Body_Exception_Handlers;
               printf ("      bodyExceptionHandlers: \n");
               processExceptionHandlerList(bodyExceptionHandlers);
               Declaration_ID           bodyBlockStatement    = declaration.Body_Block_Statement;
               printf ("      bodyBlockStatement = %d \n",bodyBlockStatement);

               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("      isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID entryIndexSpecification = declaration.Entry_Index_Specification;
               printf ("      entryIndexSpecification = %d \n",entryIndexSpecification);
               Expression_ID entryBarrier = declaration.Entry_Barrier;
               printf ("      entryBarrier = %d \n",entryBarrier);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Function_Body_Stub:
       // case A_Generic_Function_Declaration:
       // case A_Formal_Function_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               Element_ID resultProfile = declaration.Result_Profile;
               printf ("      resultProfile = %d \n",resultProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingSubunit = declaration.Corresponding_Subunit;
               printf ("      correspondingSubunit = %d \n",correspondingSubunit);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Generic_Function_Declaration:
       // case A_Formal_Function_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               Element_ID resultProfile = declaration.Result_Profile;
               printf ("      resultProfile = %d \n",resultProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Element_ID_List & genericFormalPart = declaration.Generic_Formal_Part;
               printf ("      genericFormalPart: \n");
               processElementList(genericFormalPart);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // case A_Function_Body_Declaration:
       // case An_Expression_Function_Declaration:
          case A_Function_Renaming_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               Element_ID resultProfile = declaration.Result_Profile;
               printf ("      resultProfile = %d \n",resultProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Expression_ID renamedEntity           = declaration.Renamed_Entity;
               printf ("      renamedEntity = %d \n",renamedEntity);
               Expression_ID correspondingBaseEntity = declaration.Corresponding_Base_Entity;
               printf ("      correspondingBaseEntity = %d \n",correspondingBaseEntity);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }


          case A_Function_Body_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               Element_ID resultProfile = declaration.Result_Profile;
               printf ("      resultProfile = %d \n",resultProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration    = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Element_ID_List        & bodyDeclarativeItems  = declaration.Body_Declarative_Items;
               printf ("      bodyDeclarativeItems: \n");
               processElementList(bodyDeclarativeItems);
               Statement_List         & bodyStatements        = declaration.Body_Statements;
               printf ("      bodyStatements: \n");
               processStatementList(bodyStatements);
               Exception_Handler_List & bodyExceptionHandlers = declaration.Body_Exception_Handlers;
               printf ("      bodyExceptionHandlers: \n");
               processExceptionHandlerList(bodyExceptionHandlers);
               Declaration_ID           bodyBlockStatement    = declaration.Body_Block_Statement;
               printf ("      bodyBlockStatement = %d \n",bodyBlockStatement);

               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("      isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               bool  isSubunit = declaration.Is_Subunit;
               printf ("      isSubunit = %s \n",isSubunit ? "true" : "false");
               Declaration_ID correspondingBodyStub = declaration.Corresponding_Body_Stub;
               printf ("      correspondingBodyStub = %d \n",correspondingBodyStub);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

            // SgUntypedScope* untypedScope = new SgUntypedScope();
               SgUntypedScope* untypedFunctionScope = new SgUntypedFunctionScope();
               ROSE_ASSERT(untypedFunctionScope != NULL);

               SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
               ROSE_ASSERT(untypedStatementList != NULL);

            // Connect IR nodes.
               untypedFunctionScope->set_statement_list(untypedStatementList);

            // Note that we can't connect the statements in the function body, until a second pass over the untyped AST.

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedFunctionScope;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedFunctionScope,element_id);

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // A_Function_Declaration,                   // 6.1(4)
       // A_Function_Body_Declaration,              // 6.3(2)
       // An_Expression_Function_Declaration,       // 6.8
       // A_Function_Renaming_Declaration,          // 8.5.4(2)
       // A_Function_Body_Stub,                     // 10.1.3(3)
       // A_Generic_Function_Declaration,           // 12.1(2)
       // A_Formal_Function_Declaration,            // 12.6(2)
       // case A_Function_Declaration:
       // case A_Function_Body_Declaration:
       // case An_Expression_Function_Declaration:
       // case A_Function_Renaming_Declaration:
       // case A_Function_Body_Stub:
       // case A_Generic_Function_Declaration:
       // case A_Formal_Function_Declaration:
             {
               Element_ID resultProfile = declaration.Result_Profile;
               printf ("      resultProfile = %d \n",resultProfile);
               break;
             }
#endif


       // A_Procedure_Body_Declaration,             // 6.3(2)
       // A_Function_Body_Declaration,              // 6.3(2)
       // A_Package_Body_Declaration,               // 7.2(2)
       // A_Task_Body_Declaration,                  // 9.1(6)
       // An_Entry_Body_Declaration,                // 9.5.2(5)
       // case A_Procedure_Body_Declaration:
       // case A_Function_Body_Declaration:
          case A_Package_Body_Declaration:
          case A_Task_Body_Declaration:
       // case An_Entry_Body_Declaration:
             {
               Element_ID_List        & bodyDeclarativeItems  = declaration.Body_Declarative_Items;
               printf ("      bodyDeclarativeItems: \n");
               processElementList(bodyDeclarativeItems);
               Statement_List         & bodyStatements        = declaration.Body_Statements;
               printf ("      bodyStatements: \n");
               processStatementList(bodyStatements);
               Exception_Handler_List & bodyExceptionHandlers = declaration.Body_Exception_Handlers;
               printf ("      bodyExceptionHandlers: \n");
               processExceptionHandlerList(bodyExceptionHandlers);
               Declaration_ID           bodyBlockStatement    = declaration.Body_Block_Statement;
               printf ("      bodyBlockStatement = %d \n",bodyBlockStatement);

               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("      isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               bool  isSubunit = declaration.Is_Subunit;
               printf ("      isSubunit = %s \n",isSubunit ? "true" : "false");
               Declaration_ID correspondingBodyStub = declaration.Corresponding_Body_Stub;
               printf ("      correspondingBodyStub = %d \n",correspondingBodyStub);

#if 1
               SgUntypedScope* untypedScope = new SgUntypedScope();
               ROSE_ASSERT(untypedScope != NULL);
#else
               SgUntypedScope* untypedScope = NULL;
               if (declarationKind == A_Package_Body_Declaration)
                  {
                    printf ("Building a global scope \n");
                    untypedScope = new SgUntypedGlobalScope();
                  }
                 else
                  {
                    untypedScope = new SgUntypedScope();
                  }
               ROSE_ASSERT(untypedScope != NULL);
#endif

               SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
               ROSE_ASSERT(untypedStatementList != NULL);

            // Connect IR nodes.
               untypedScope->set_statement_list(untypedStatementList);

            // Note that we can't connect the statements in the function body, until a second pass over the untyped AST.

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedScope;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedScope,element_id);

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Function_Declaration:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("      traitKind (value) = %d \n",traitKind);
            // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               Element_ID resultProfile = declaration.Result_Profile;
               printf ("      resultProfile = %d \n",resultProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration    = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Declaration_ID correspondingSubprogramDerivation = declaration.Corresponding_Subprogram_Derivation;
               printf ("      correspondingSubprogramDerivation = %d \n",correspondingSubprogramDerivation);

               Type_Definition_ID correspondingType = declaration.Corresponding_Type;
               printf ("      correspondingType = %d \n",correspondingType);

               Declaration_ID correspondingEqualityOperator = declaration.Corresponding_Equality_Operator;
               printf ("      correspondingEqualityOperator = %d \n",correspondingEqualityOperator);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

            // Get the name from the name list.
               ROSE_ASSERT(names.Length == 1);
               int functionName_id = names.IDs[0];

               printf ("functionName_id = %d \n",functionName_id);

               ROSE_ASSERT(untypedNodeMap.find(functionName_id) != untypedNodeMap.end());
               SgUntypedNode* untypedNode = untypedNodeMap[functionName_id];
               ROSE_ASSERT(untypedNode != NULL);
            // SgUntypedInitializedName* untypedInitializedName = isSgUntypedInitializedName(untypedNode);
            // ROSE_ASSERT(untypedInitializedName != NULL);
            // string functionName = untypedInitializedName->get_name();
               SgUntypedName* untypedName = isSgUntypedName(untypedNode);
               ROSE_ASSERT(untypedName != NULL);
               string functionName = untypedName->get_name();

               printf ("functionName = %s \n",functionName.c_str());

               SgUntypedFunctionDeclaration* untypedFunctionDeclaration = new SgUntypedFunctionDeclaration(functionName);
               ROSE_ASSERT(untypedFunctionDeclaration != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedFunctionDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedFunctionDeclaration,element_id);

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Expression_Function_Declaration,       // 6.8
          case An_Expression_Function_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               Element_ID resultProfile = declaration.Result_Profile;
               printf ("      resultProfile = %d \n",resultProfile);

               Expression_ID resultExpression = declaration.Result_Expression;
               printf ("      resultExpression = %d \n",resultExpression);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration    = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Type_Definition_ID correspondingType = declaration.Corresponding_Type;
               printf ("      correspondingType = %d \n",correspondingType);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Procedure_Declaration,                  // 6.1(4)
       // A_Function_Declaration,                   // 6.1(4)
       // A_Procedure_Body_Declaration,             // 6.3(2)
       // A_Function_Body_Declaration,              // 6.3(2)
       // A_Null_Procedure_Declaration,             // 6.7
       // An_Expression_Function_Declaration,       // 6.8
       // A_Procedure_Renaming_Declaration,         // 8.5.4(2)
       // A_Function_Renaming_Declaration,          // 8.5.4(2)
       // An_Entry_Declaration,                     // 9.5.2(2)
       // A_Procedure_Body_Stub,                    // 10.1.3(3)
       // A_Function_Body_Stub,                     // 10.1.3(3)
       // A_Generic_Procedure_Declaration,          // 12.1(2)
       // A_Generic_Function_Declaration,           // 12.1(2)
       // A_Formal_Procedure_Declaration,           // 12.6(2)
       // A_Formal_Function_Declaration,            // 12.6(2)
       // case A_Procedure_Declaration:
       // case A_Function_Declaration:
       // case A_Procedure_Body_Declaration:
       // case A_Function_Body_Declaration:

       // case A_Null_Procedure_Declaration:
       // case An_Expression_Function_Declaration:
       // case A_Procedure_Renaming_Declaration:
       // case An_Entry_Declaration:
       // case A_Procedure_Body_Stub:
       // case A_Function_Body_Stub:
       // case A_Generic_Procedure_Declaration:
       // case A_Generic_Function_Declaration:
          case A_Formal_Procedure_Declaration:
       // case A_Formal_Function_Declaration:
             {
               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Expression_ID formalSubprogramDefault = declaration.Formal_Subprogram_Default;
               printf ("      formalSubprogramDefault = %d \n",formalSubprogramDefault);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
       // A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
       // A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
       // A_Procedure_Body_Declaration,             // 6.3(2)
       // A_Function_Body_Declaration,              // 6.3(2)
       // A_Package_Declaration,                    // 7.1(2)
       // A_Package_Body_Declaration,               // 7.2(2)
       // A_Task_Body_Declaration,                  // 9.1(6)
       // A_Protected_Body_Declaration,             // 9.4(7)
       // An_Entry_Body_Declaration,                // 9.5.2(5)
       // A_Generic_Package_Declaration,            // 12.1(2)
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
       // case A_Single_Task_Declaration:
       // case A_Single_Protected_Declaration:
       // case A_Procedure_Body_Declaration:
       // case A_Function_Body_Declaration:
       // case A_Package_Declaration:
       // case A_Package_Body_Declaration:
       // case A_Task_Body_Declaration:
          case A_Protected_Body_Declaration:
       // case An_Entry_Body_Declaration:
       // case A_Generic_Package_Declaration:
             {
               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_List & protectedOperationItems = declaration.Protected_Operation_Items;
               printf ("protectedOperationItems: \n");
               processDeclarationList(protectedOperationItems);

               bool  isSubunit = declaration.Is_Subunit;
               printf ("isSubunit = %s \n",isSubunit ? "true" : "false");
               Declaration_ID correspondingBodyStub = declaration.Corresponding_Body_Stub;
               printf ("correspondingBodyStub = %d \n",correspondingBodyStub);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Package_Declaration:
             {
               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declarative_Item_List & visiblePartDeclarativeItems  = declaration.Visible_Part_Declarative_Items;
               printf ("visiblePartDeclarativeItems: \n");
               processDeclarativeItemList(visiblePartDeclarativeItems);

               bool                    isPrivatePresent              = declaration.Is_Private_Present;
               printf ("      isPrivatePresent = %s \n",isPrivatePresent ? "true" : "false");

               Declarative_Item_List & privatePartDeclarativeItems  = declaration.Private_Part_Declarative_Items;
               printf ("privatePartDeclarativeItems: \n");
               processDeclarativeItemList(privatePartDeclarativeItems);

            // ********************************
            // This needs to force the declaration of a function (with function definition, and body).
               printf ("In processDeclaration(): case A_Package_Declaration: (not implemented) \n");

            // Get the name from the name list.
               ROSE_ASSERT(names.Length == 1);
               int packageName_id = names.IDs[0];

               printf ("packageName_id = %d \n",packageName_id);

               ROSE_ASSERT(untypedNodeMap.find(packageName_id) != untypedNodeMap.end());
               SgUntypedNode* untypedNode = untypedNodeMap[packageName_id];
               ROSE_ASSERT(untypedNode != NULL);
            // SgUntypedInitializedName* untypedInitializedName = isSgUntypedInitializedName(untypedNode);
            // ROSE_ASSERT(untypedInitializedName != NULL);
            // string packageName = untypedInitializedName->get_name();
               SgUntypedName* untypedName = isSgUntypedName(untypedNode);
               ROSE_ASSERT(untypedName != NULL);
               string packageName = untypedName->get_name();

               printf ("packageName = %s \n",packageName.c_str());

               SgUntypedPackageDeclaration* untypedPackageDeclaration = new SgUntypedPackageDeclaration(packageName);
               ROSE_ASSERT(untypedPackageDeclaration != NULL);

               SgUntypedScope* untypedScope = new SgUntypedScope();
               ROSE_ASSERT(untypedScope != NULL);

               untypedPackageDeclaration->set_scope(untypedScope);

               SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
               ROSE_ASSERT(untypedStatementList != NULL);

            // Connect IR nodes.
               untypedScope->set_statement_list(untypedStatementList);

            // Note that we can't connect the statements in the function body, until a second pass over the untyped AST.

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedPackageDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedPackageDeclaration,element_id);
            // ********************************

// #if DEBUG_UNTYPED_NODE_GENERATION
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Generic_Package_Declaration:
             {
               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Declarative_Item_List & visiblePartDeclarativeItems  = declaration.Visible_Part_Declarative_Items;
               printf ("visiblePartDeclarativeItems: \n");
               processDeclarativeItemList(visiblePartDeclarativeItems);

               bool                    isPrivatePresent              = declaration.Is_Private_Present;
               printf ("      isPrivatePresent = %s \n",isPrivatePresent ? "true" : "false");

               Declarative_Item_List & privatePartDeclarativeItems  = declaration.Private_Part_Declarative_Items;
               printf ("privatePartDeclarativeItems: \n");
               processDeclarativeItemList(privatePartDeclarativeItems);

               Element_ID_List & genericFormalPart = declaration.Generic_Formal_Part;
               printf ("genericFormalPart: \n");
               processElementList(genericFormalPart);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
       // A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
       // A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
       // A_Procedure_Declaration,                  // 6.1(4)
       // A_Function_Declaration,                   // 6.1(4)
       // A_Procedure_Body_Declaration,             // 6.3(2)
       // A_Function_Body_Declaration,              // 6.3(2)
       // A_Null_Procedure_Declaration,             // 6.7
       // An_Expression_Function_Declaration,       // 6.8
       // A_Package_Declaration,                    // 7.1(2)
       // A_Package_Body_Declaration,               // 7.2(2)
       // A_Package_Renaming_Declaration,           // 8.5.3(2)
       // A_Procedure_Renaming_Declaration,         // 8.5.4(2)
       // A_Function_Renaming_Declaration,          // 8.5.4(2)
       // A_Generic_Package_Renaming_Declaration,   // 8.5.5(2)
       // A_Generic_Procedure_Renaming_Declaration, // 8.5.5(2)
       // A_Generic_Function_Renaming_Declaration,  // 8.5.5(2)
       // A_Task_Body_Declaration,                  // 9.1(6)
       // A_Protected_Body_Declaration,             // 9.4(7)
       // An_Entry_Body_Declaration,                // 9.5.2(5)
       // An_Entry_Index_Specification,             // 9.5.2(2)
       // A_Procedure_Body_Stub,                    // 10.1.3(3)
       // A_Function_Body_Stub,                     // 10.1.3(3)
       // A_Package_Body_Stub,                      // 10.1.3(4)
       // A_Task_Body_Stub,                         // 10.1.3(5)
       // A_Protected_Body_Stub,                    // 10.1.3(6)
       // A_Generic_Procedure_Declaration,          // 12.1(2)
       // A_Generic_Function_Declaration,           // 12.1(2)
       // A_Generic_Package_Declaration,            // 12.1(2)
       // A_Package_Instantiation,                  // 12.3(2)
       // A_Procedure_Instantiation,                // 12.3(2)
       // A_Function_Instantiation,                 // 12.3(2)
       // A_Formal_Package_Declaration,             // 12.7(2)
       // A_Formal_Package_Declaration_With_Box     // 12.7(3)
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
       // case A_Single_Task_Declaration:
       // case A_Single_Protected_Declaration:
       // case A_Procedure_Declaration:
       // case A_Function_Declaration:
       // case A_Procedure_Body_Declaration:
       // case A_Function_Body_Declaration:
       // case A_Null_Procedure_Declaration:
       // case An_Expression_Function_Declaration:
       // case A_Package_Declaration:
       // case A_Package_Body_Declaration:
       // case A_Package_Renaming_Declaration:
       // case A_Procedure_Renaming_Declaration:
       // case A_Function_Renaming_Declaration:
       // case A_Generic_Package_Renaming_Declaration:
       // case A_Generic_Procedure_Renaming_Declaration:
       // case A_Generic_Function_Renaming_Declaration:
       // case A_Task_Body_Declaration:
       // case A_Protected_Body_Declaration:
       // case An_Entry_Body_Declaration:
       // case An_Entry_Index_Specification:
       // case A_Procedure_Body_Stub:
       // case A_Function_Body_Stub:
       // case A_Package_Body_Stub:
       // case A_Task_Body_Stub:
       // case A_Protected_Body_Stub:
       // case A_Generic_Procedure_Declaration:
       // case A_Generic_Function_Declaration:
       // case A_Generic_Package_Declaration:
       // case A_Package_Instantiation:
       // case A_Procedure_Instantiation:
       // case A_Function_Instantiation:
       // case A_Formal_Package_Declaration:
          case A_Formal_Package_Declaration_With_Box:
             {
               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Expression_ID  genericUnitName = declaration.Generic_Unit_Name;
               printf ("genericUnitName = %d \n",genericUnitName);
               Association_List & genericActualPart = declaration.Generic_Actual_Part;
               printf ("genericActualPart: \n");
               processAssociationList(genericActualPart);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Package_Body_Stub:
          case A_Task_Body_Stub:
          case A_Protected_Body_Stub:
             {
               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingSubunit = declaration.Corresponding_Subunit;
               printf ("      correspondingSubunit = %d \n",correspondingSubunit);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Generic_Package_Renaming_Declaration:
          case A_Generic_Procedure_Renaming_Declaration:
          case A_Generic_Function_Renaming_Declaration:
             {
               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Expression_ID renamedEntity           = declaration.Renamed_Entity;
               printf ("      renamedEntity = %d \n",renamedEntity);
               Expression_ID correspondingBaseEntity = declaration.Corresponding_Base_Entity;
               printf ("      correspondingBaseEntity = %d \n",correspondingBaseEntity);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }


          case A_Package_Renaming_Declaration:
             {
               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Expression_ID renamedEntity           = declaration.Renamed_Entity;
               printf ("      renamedEntity = %d \n",renamedEntity);
               Expression_ID correspondingBaseEntity = declaration.Corresponding_Base_Entity;
               printf ("      correspondingBaseEntity = %d \n",correspondingBaseEntity);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Package_Instantiation:
          case A_Procedure_Instantiation:
          case A_Function_Instantiation:
          case A_Formal_Package_Declaration:
             {
               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Expression_ID  genericUnitName = declaration.Generic_Unit_Name;
               printf ("genericUnitName = %d \n",genericUnitName);
               Association_List & genericActualPart = declaration.Generic_Actual_Part;
               printf ("genericActualPart: \n");
               processAssociationList(genericActualPart);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Entry_Index_Specification:
             {
               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
       // A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
       // A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
       // A_Procedure_Declaration,                  // 6.1(4)
       // A_Function_Declaration,                   // 6.1(4)
       // An_Entry_Declaration,                     // 9.5.2(2)
       // An_Entry_Index_Specification,             // 9.5.2(2)
       // A_Generic_Procedure_Declaration,          // 12.1(2)
       // A_Generic_Function_Declaration,           // 12.1(2)
       // A_Generic_Package_Declaration,            // 12.1(2)
       // A_Package_Instantiation,                  // 12.3(2)
       // A_Procedure_Instantiation,                // 12.3(2)
       // A_Function_Instantiation,                 // 12.3(2)
       // A_Formal_Package_Declaration,             // 12.7(2)
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
       // case A_Single_Task_Declaration:
       // case A_Single_Protected_Declaration:
       // case A_Procedure_Declaration:
       // case A_Function_Declaration:
       // case An_Entry_Declaration:
       // case An_Entry_Index_Specification:
       // case A_Generic_Procedure_Declaration:
       // case A_Generic_Function_Declaration:
       // case A_Generic_Package_Declaration:
       // case A_Package_Instantiation:
       // case A_Procedure_Instantiation:
       // case A_Function_Instantiation:
       // case A_Formal_Package_Declaration:
             {
               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);
               break;
             }
#endif

#if 0
       // A_Procedure_Declaration,                  // 6.1(4)
       // A_Function_Declaration,                   // 6.1(4)
       // case A_Procedure_Declaration:
       // case A_Function_Declaration:
             {
               Declaration_ID correspondingSubprogramDerivation = declaration.Corresponding_Subprogram_Derivation;
               printf ("      correspondingSubprogramDerivation = %d \n",correspondingSubprogramDerivation);
               break;
             }
#endif
#if 0
       // A_Procedure_Declaration,                  // 6.1(4)
       // A_Function_Declaration,                   // 6.1(4)
       // An_Expression_Function_Declaration,       // 6.8
       // case A_Procedure_Declaration:
       // case A_Function_Declaration:
       // case An_Expression_Function_Declaration:
             {
               Type_Definition_ID correspondingType = declaration.Corresponding_Type;
               printf ("      correspondingType = %d \n",correspondingType);
               break;
             }
#endif
#if 0
       // A_Function_Declaration,                   // 6.1(4)
       // case A_Function_Declaration:
             {
               Declaration_ID correspondingEqualityOperator = declaration.Corresponding_Equality_Operator;
               printf ("      correspondingEqualityOperator = %d \n",correspondingEqualityOperator);
               break;
             }
#endif
#if 0
       // A_Package_Declaration,                    // 7.1(2)
       // A_Generic_Package_Declaration,            // 12.1(2)
       // case A_Package_Declaration:
       // case A_Generic_Package_Declaration:
             {
               Declarative_Item_List & visiblePartDeclarativeItems  = declaration.Visible_Part_Declarative_Items;
               printf ("visiblePartDeclarativeItems: \n");
               processDeclarativeItemList(visiblePartDeclarativeItems);

               bool                    isPrivatePresent              = declaration.Is_Private_Present;
               printf ("      isPrivatePresent = %s \n",isPrivatePresent ? "true" : "false");

               Declarative_Item_List & privatePartDeclarativeItems  = declaration.Private_Part_Declarative_Items;
               printf ("privatePartDeclarativeItems: \n");
               processDeclarativeItemList(privatePartDeclarativeItems);
               break;
             }
#endif
#if 0
       // A_Task_Type_Declaration,                  // 9.1(2)
       // A_Protected_Type_Declaration,             // 9.4(2)
       // A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
       // A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)
       // case A_Task_Type_Declaration:
       // case A_Protected_Type_Declaration:
       // case A_Single_Task_Declaration:
       // case A_Single_Protected_Declaration:
             {
               Expression_List & declarationInterfaceList = declaration.Declaration_Interface_List;
               printf ("declarationInterfaceList \n");
               processExpressionList(declarationInterfaceList);
               break;
             }
#endif

       // An_Object_Renaming_Declaration,           // 8.5.1(2)
       // An_Exception_Renaming_Declaration,        // 8.5.2(2)
       // A_Package_Renaming_Declaration,           // 8.5.3(2)
       // A_Procedure_Renaming_Declaration,         // 8.5.4(2)
       // A_Function_Renaming_Declaration,          // 8.5.4(2)
       // A_Generic_Package_Renaming_Declaration,   // 8.5.5(2)
       // A_Generic_Procedure_Renaming_Declaration, // 8.5.5(2)
       // A_Generic_Function_Renaming_Declaration,  // 8.5.5(2)
       // case An_Object_Renaming_Declaration:
          case An_Exception_Renaming_Declaration:
       // case A_Package_Renaming_Declaration:
       // case A_Procedure_Renaming_Declaration:
       // case A_Function_Renaming_Declaration:
       // case A_Generic_Package_Renaming_Declaration:
       // case A_Generic_Procedure_Renaming_Declaration:
       // case A_Generic_Function_Renaming_Declaration:
             {
               Expression_ID renamedEntity           = declaration.Renamed_Entity;
               printf ("      renamedEntity = %d \n",renamedEntity);
               Expression_ID correspondingBaseEntity = declaration.Corresponding_Base_Entity;
               printf ("      correspondingBaseEntity = %d \n",correspondingBaseEntity);

#if DEBUG_UNTYPED_NODE_GENERATION
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // A_Protected_Body_Declaration,             // 9.4(7)
       // case A_Protected_Body_Declaration:
             {
               Declaration_List & protectedOperationItems = declaration.Protected_Operation_Items;
               printf ("protectedOperationItems: \n");
               processDeclarationList(protectedOperationItems);
               break;
             }
#endif
#if 0
       // An_Entry_Declaration,                     // 9.5.2(2)
       // case An_Entry_Declaration:
             {
               Discrete_Subtype_Definition_ID entryFamilyDefinition = declaration.Entry_Family_Definition;
               printf ("      entryFamilyDefinition = %d \n",entryFamilyDefinition);
               break;
             }
#endif
#if 0
       // An_Entry_Body_Declaration,                // 9.5.2(5)
       // case An_Entry_Body_Declaration:
             {
               Declaration_ID entryIndexSpecification = declaration.Entry_Index_Specification;
               printf ("      entryIndexSpecification = %d \n",entryIndexSpecification);
               Expression_ID entryBarrier = declaration.Entry_Barrier;
               printf ("      entryBarrier = %d \n",entryBarrier);
               break;
             }
#endif
#if 0
       // A_Procedure_Body_Stub,                    // 10.1.3(3)
       // A_Function_Body_Stub,                     // 10.1.3(3)
       // A_Package_Body_Stub,                      // 10.1.3(4)
       // A_Task_Body_Stub,                         // 10.1.3(5)
       // A_Protected_Body_Stub,                    // 10.1.3(6)
       // case A_Procedure_Body_Stub:
       // case A_Function_Body_Stub:
       // case A_Package_Body_Stub:
       // case A_Task_Body_Stub:
       // case A_Protected_Body_Stub:
             {
               Declaration_ID correspondingSubunit = declaration.Corresponding_Subunit;
               printf ("      correspondingSubunit = %d \n",correspondingSubunit);
               break;
             }
#endif
#if 0
       // A_Procedure_Body_Declaration,             // 6.3(2)
       // A_Function_Body_Declaration,              // 6.3(2)
       // A_Package_Body_Declaration,               // 7.2(2)
       // A_Task_Body_Declaration,                  // 9.1(6)
       // A_Protected_Body_Declaration,             // 9.4(7)
       // case A_Procedure_Body_Declaration:
       // case A_Function_Body_Declaration:
       // case A_Package_Body_Declaration:
       // case A_Task_Body_Declaration:
       // case A_Protected_Body_Declaration:
             {
               bool  isSubunit = declaration.Is_Subunit;
               printf ("isSubunit = %s \n",isSubunit ? "true" : "false");
               Declaration_ID correspondingBodyStub = declaration.Corresponding_Body_Stub;
               printf ("correspondingBodyStub = %d \n",correspondingBodyStub);
               break;
             }
#endif
#if 0
       // A_Generic_Procedure_Declaration,          // 12.1(2)
       // A_Generic_Function_Declaration,           // 12.1(2)
       // A_Generic_Package_Declaration,            // 12.1(2)
       // case A_Generic_Procedure_Declaration:
       // case A_Generic_Function_Declaration:
       // case A_Generic_Package_Declaration:
             {
               Element_List & genericFormalPart = declaration.Generic_Formal_Part;
               printf ("genericFormalPart: \n");
               processElementList(genericFormalPart);
               break;
             }
#endif
#if 0
       // A_Package_Instantiation,                  // 12.3(2)
       // A_Procedure_Instantiation,                // 12.3(2)
       // A_Function_Instantiation,                 // 12.3(2)
       // A_Formal_Package_Declaration,             // 12.7(2)
       // A_Formal_Package_Declaration_With_Box     // 12.7(3)
       // case A_Package_Instantiation:
       // case A_Procedure_Instantiation:
       // case A_Function_Instantiation:
       // case A_Formal_Package_Declaration:
       // case A_Formal_Package_Declaration_With_Box:
             {
               Expression_ID  genericUnitName = declaration.Generic_Unit_Name;
               printf ("genericUnitName = %d \n",genericUnitName);
               Association_List & genericActualPart = declaration.Generic_Actual_Part;
               printf ("genericActualPart: \n",);
               processAssociationList(genericActualPart);
               break;
             }
#endif
#if 0
       // A_Formal_Procedure_Declaration,           // 12.6(2)
       // A_Formal_Function_Declaration,            // 12.6(2)
       // case A_Formal_Procedure_Declaration:
       // case A_Formal_Function_Declaration:
             {
               Expression_ID formalSubprogramDefault = declaration.Formal_Subprogram_Default;
               printf ("formalSubprogramDefault = %d \n",formalSubprogramDefault);
               break;
             }
#endif
#if 0
       // A_Procedure_Declaration,                  // 6.1(4)
       // A_Function_Declaration,                   // 6.1(4)
       // A_Procedure_Body_Declaration,             // 6.3(2)
       // A_Function_Body_Declaration,              // 6.3(2)
       // A_Null_Procedure_Declaration,             // 6.7
       // An_Expression_Function_Declaration,       // 6.8
       // A_Procedure_Renaming_Declaration,         // 8.5.4(2)
       // A_Function_Renaming_Declaration,          // 8.5.4(2)
       // A_Procedure_Body_Stub,                    // 10.1.3(3)
       // A_Function_Body_Stub,                     // 10.1.3(3)
       // case A_Procedure_Declaration:
       // case A_Function_Declaration:
       // case A_Procedure_Body_Declaration:
       // case A_Function_Body_Declaration:
       // case A_Null_Procedure_Declaration:
       // case An_Expression_Function_Declaration:
       // case A_Procedure_Renaming_Declaration:
       // case A_Function_Renaming_Declaration:
       // case A_Procedure_Body_Stub:
       // case A_Function_Body_Stub:
             {
               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");
               break;
             }
#endif

          case A_Procedure_Declaration:
       // case A_Function_Declaration:
             {
            // Trait_Kinds traitKind = declaration.Trait_Kind;
            // printf ("      traitKind (value) = %d \n",traitKind);
            // printf ("      traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration    = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               Declaration_ID correspondingBody = declaration.Corresponding_Body;
               printf ("      correspondingBody = %d \n",correspondingBody);

               Declaration_ID correspondingSubprogramDerivation = declaration.Corresponding_Subprogram_Derivation;
               printf ("      correspondingSubprogramDerivation = %d \n",correspondingSubprogramDerivation);

               Type_Definition_ID correspondingType = declaration.Corresponding_Type;
               printf ("      correspondingType = %d \n",correspondingType);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

            // An example of this can be a function prototype (see unit_3.ads).

            // This needs to force the declaration of a function (with function definition, and body).
               printf ("In processDeclaration(): case A_Procedure_Declaration: (not implemented) \n");

            // Get the name from the name list.
               ROSE_ASSERT(names.Length == 1);
               int functionName_id = names.IDs[0];

               printf ("functionName_id = %d \n",functionName_id);

               ROSE_ASSERT(untypedNodeMap.find(functionName_id) != untypedNodeMap.end());
               SgUntypedNode* untypedNode = untypedNodeMap[functionName_id];
               ROSE_ASSERT(untypedNode != NULL);
            // SgUntypedInitializedName* untypedInitializedName = isSgUntypedInitializedName(untypedNode);
            // ROSE_ASSERT(untypedInitializedName != NULL);
            // string functionName = untypedInitializedName->get_name();
               SgUntypedName* untypedName = isSgUntypedName(untypedNode);
               ROSE_ASSERT(untypedName != NULL);
               string functionName = untypedName->get_name();

               printf ("functionName = %s \n",functionName.c_str());

               SgUntypedFunctionDeclaration* untypedFunctionDeclaration = new SgUntypedFunctionDeclaration(functionName);
               ROSE_ASSERT(untypedFunctionDeclaration != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedFunctionDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedFunctionDeclaration,element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Procedure_Body_Declaration:
             {
               Parameter_Specification_List & parameterProfile = declaration.Parameter_Profile;
               printf ("      parameterProfile: \n");
               processParameterSpecificationList(parameterProfile);

               bool isOverridingDeclaration     = declaration.Is_Overriding_Declaration;
               bool isNotOverridingDeclaration = declaration.Is_Not_Overriding_Declaration;
               printf ("      isOverridingDeclaration    = %s \n",isOverridingDeclaration ? "true" : "false");
               printf ("      isNotOverridingDeclaration = %s \n",isNotOverridingDeclaration ? "true" : "false");

               Element_ID_List           & bodyDeclarativeItems  = declaration.Body_Declarative_Items;
               printf ("      bodyDeclarativeItems: \n");
               processElementList(bodyDeclarativeItems);
               Statement_List         & bodyStatements        = declaration.Body_Statements;
               printf ("      bodyStatements: \n");
               processStatementList(bodyStatements);
               Exception_Handler_List & bodyExceptionHandlers = declaration.Body_Exception_Handlers;
               printf ("      bodyExceptionHandlers: \n");
               processExceptionHandlerList(bodyExceptionHandlers);
               Declaration_ID           bodyBlockStatement    = declaration.Body_Block_Statement;
               printf ("      bodyBlockStatement = %d \n",bodyBlockStatement);

               bool isNameRepeated = declaration.Is_Name_Repeated;
               printf ("      isNameRepeated = %s \n",isNameRepeated ? "true" : "false");

               Declaration_ID correspondingDeclaration = declaration.Corresponding_Declaration;
               printf ("      correspondingDeclaration = %d \n",correspondingDeclaration);

               bool  isSubunit = declaration.Is_Subunit;
               printf ("      isSubunit = %s \n",isSubunit ? "true" : "false");
               Declaration_ID correspondingBodyStub = declaration.Corresponding_Body_Stub;
               printf ("      correspondingBodyStub = %d \n",correspondingBodyStub);

               bool isDispatchingOperation = declaration.Is_Dispatching_Operation;
               printf ("      isDispatchingOperation = %s \n",isDispatchingOperation ? "true" : "false");

            // This needs to force the declaration of a function (with function definition, and body).
               printf ("In processDeclaration(): case A_Procedure_Body_Declaration: (not implemented) \n");

            // Get the name from the name list.
               ROSE_ASSERT(names.Length == 1);
               int functionName_id = names.IDs[0];

               printf ("functionName_id = %d \n",functionName_id);

               ROSE_ASSERT(untypedNodeMap.find(functionName_id) != untypedNodeMap.end());
               SgUntypedNode* untypedNode = untypedNodeMap[functionName_id];
               ROSE_ASSERT(untypedNode != NULL);
            // SgUntypedInitializedName* untypedInitializedName = isSgUntypedInitializedName(untypedNode);
            // ROSE_ASSERT(untypedInitializedName != NULL);
            // string functionName = untypedInitializedName->get_name();
               SgUntypedName* untypedName = isSgUntypedName(untypedNode);
               ROSE_ASSERT(untypedName != NULL);
               string functionName = untypedName->get_name();

               printf ("functionName = %s \n",functionName.c_str());

               SgUntypedFunctionDeclaration* untypedFunctionDeclaration = new SgUntypedFunctionDeclaration(functionName);
               ROSE_ASSERT(untypedFunctionDeclaration != NULL);

               SgUntypedFunctionScope* untypedFunctionScope = new SgUntypedFunctionScope();
               ROSE_ASSERT(untypedFunctionScope != NULL);

               untypedFunctionDeclaration->set_scope(untypedFunctionScope);

               SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
               ROSE_ASSERT(untypedStatementList != NULL);

            // Connect IR nodes.
               untypedFunctionScope->set_statement_list(untypedStatementList);

            // Note that we can't connect the statements in the function body, until a second pass over the untyped AST.

#if 0
            // SgUntypedStatement* untypedStatement = new SgUntypedBlockStatement();

            // Put into map using Ada Node ID's as the keys.
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedStatement;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               printf ("untypedNodeMap.size() = %zu \n",untypedNodeMap.size());

            // Build the attribute (element id onto the SgUntypedNode).
               setAsisAttribute (untypedStatement,element_id);
#else
               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedFunctionDeclaration;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedFunctionDeclaration,element_id);
#endif

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case  An_Exception_Declaration:
             {
            // No data members for this case.

            // However, we still need to build a exception untyped IR node.
               SgUntypedExceptionDeclaration* untypedExceptionDeclaration = new SgUntypedExceptionDeclaration();
               ROSE_ASSERT(untypedExceptionDeclaration != NULL);

               processUntypedNode(untypedExceptionDeclaration,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Choice_Parameter_Specification:
             {
            // No data members for this case yet.
               break;
             }

          default:
             {
               printf ("Default reached in processDeclaration(): declarationKind = %d declarationKind = %s \n",declarationKind,declarationKindName(declarationKind).c_str());
               ROSE_ASSERT(false);
             }
        }

#if 0
     printf ("      Discriminant_Part = %d \n",declaration.Discriminant_Part);
     printf ("      Type_Declaration_View = %d \n",declaration.Type_Declaration_View);
     printf ("      Object_Declaration_View = %d \n",declaration.Object_Declaration_View);
     printf ("   Aspect_Specifications: \n");
     processElementList(declaration.Aspect_Specifications);
     printf ("      Initialization_Expression = %d \n",declaration.Initialization_Expression);
     printf ("      Corresponding_Type_Declaration = %d \n",declaration.Corresponding_Type_Declaration);
     printf ("      Corresponding_Type_Completion = %d \n",declaration.Corresponding_Type_Completion);
     printf ("      Corresponding_Type_Partial_View = %d \n",declaration.Corresponding_Type_Partial_View);
     printf ("      Corresponding_First_Subtype = %d \n",declaration.Corresponding_First_Subtype);
  // Should there be a first constraint?  Charles double checked and the answer is NO.
     printf ("      Corresponding_Last_Constraint = %d \n",declaration.Corresponding_Last_Constraint);
     printf ("      Corresponding_Last_Subtype = %d \n",declaration.Corresponding_Last_Subtype);
     printf ("   Corresponding_Representation_Clauses: \n");
     processRepresentationClauseList(declaration.Corresponding_Representation_Clauses);
     printf ("      Specification_Subtype_Definition = %d \n",declaration.Specification_Subtype_Definition);
     printf ("      Iteration_Scheme_Name = %d \n",declaration.Iteration_Scheme_Name);
     printf ("      Subtype_Indication = %d \n",declaration.Subtype_Indication);
     printf ("   Parameter_Profile: \n");
     processParameterSpecificationList (declaration.Parameter_Profile);
     printf ("      Result_Profile = %d \n",declaration.Result_Profile);
     printf ("      Result_Expression = %d \n",declaration.Result_Expression);
     printf ("      Is_Overriding_Declaration     = %s \n",declaration.Is_Overriding_Declaration     ? "true" : "false");
     printf ("      Is_Not_Overriding_Declaration = %s \n",declaration.Is_Not_Overriding_Declaration ? "true" : "false");
     printf ("   Body_Declarative_Items: \n");
     processElementList (declaration.Body_Declarative_Items);
     printf ("   Body_Statements: \n");
     processStatementList (declaration.Body_Statements);
     printf ("   Body_Exception_Handlers: \n");
     processExceptionHandlerList (declaration.Body_Exception_Handlers);
     printf ("      Body_Block_Statement = %d \n",declaration.Body_Block_Statement);
     printf ("      Is_Name_Repeated     = %s \n",declaration.Is_Name_Repeated ? "true" : "false");
     printf ("      Corresponding_Declaration = %d \n",declaration.Corresponding_Declaration);
     printf ("      Corresponding_Body = %d \n",declaration.Corresponding_Body);
     printf ("      Corresponding_Subprogram_Derivation = %d \n",declaration.Corresponding_Subprogram_Derivation);
     printf ("      Corresponding_Type = %d \n",declaration.Corresponding_Type);
     printf ("      Corresponding_Equality_Operator = %d \n",declaration.Corresponding_Equality_Operator);
     printf ("   Visible_Part_Declarative_Items: \n");
     processDeclarativeItemList (declaration.Visible_Part_Declarative_Items);
     printf ("      Is_Private_Present = %s \n",declaration.Is_Private_Present  ? "true" : "false");
     printf ("   Private_Part_Declarative_Items: \n");
     processDeclarativeItemList (declaration.Private_Part_Declarative_Items);
     printf ("   Declaration_Interface_List: \n");
     processExpressionList (declaration.Declaration_Interface_List);
     printf ("      Renamed_Entity = %d \n",declaration.Renamed_Entity);
     printf ("      Corresponding_Base_Entity = %d \n",declaration.Corresponding_Base_Entity);
     printf ("   Protected_Operation_Items: \n");
     processDeclarationList (declaration.Protected_Operation_Items);
     printf ("      Entry_Family_Definition   = %d \n",declaration.Entry_Family_Definition);
     printf ("      Entry_Index_Specification = %d \n",declaration.Entry_Index_Specification);
     printf ("      Entry_Barrier = %d \n",declaration.Entry_Barrier);
     printf ("      Corresponding_Subunit = %d \n",declaration.Corresponding_Subunit);
     printf ("      Is_Subunit = %s \n",declaration.Is_Subunit  ? "true" : "false");
     printf ("      Corresponding_Body_Stub = %d \n",declaration.Corresponding_Body_Stub);
     printf ("   Generic_Formal_Part: \n");
     processElementList (declaration.Generic_Formal_Part);
     printf ("      Generic_Unit_Name = %d \n",declaration.Generic_Unit_Name);
     printf ("   Generic_Actual_Part: \n");
     processAssociationList (declaration.Generic_Actual_Part);
     printf ("      Formal_Subprogram_Default = %d \n",declaration.Formal_Subprogram_Default);
     printf ("      Is_Dispatching_Operation = %s \n",declaration.Is_Dispatching_Operation  ? "true" : "false");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }





void
  Ada_ROSE_Translation::processExpression( Expression_Struct & expression, int element_id)
   {
#if 0
  // Documentation for Expression_Struct.
  enum Expression_Kinds Expression_Kind;
  Declaration_ID        Corresponding_Expression_Type;

  // These fields are only valid for the kinds above them:
  // An_Integer_Literal,                        // 2.4
  // A_Real_Literal,                            // 2.4.1
  // A_String_Literal,                          // 2.6
  char                 *Value_Image;
  // An_Identifier |                              // 4.1
  // An_Operator_Symbol |                         // 4.1
  // A_Character_Literal |                        // 4.1
  // An_Enumeration_Literal:
  char                 *Name_Image;
  Defining_Name_ID      Corresponding_Name_Definition;
  Defining_Name_List    Corresponding_Name_Definition_List; // Only >1 if the expression in a pragma is ambiguous
  Element_ID            Corresponding_Name_Declaration; // Decl or stmt
  // An_Operator_Symbol:
  enum Operator_Kinds   Operator_Kind;
  // An_Explicit_Dereference =>                   // 4.1
  // A_Function_Call =>                           // 4.1
  // An_Indexed_Component =>                      // 4.1.1
  // A_Slice =>                                   // 4.1.2
  // A_Selected_Component =>                      // 4.1.3
  // An_Attribute_Reference =>                    // 4.1.4
  Expression_ID         Prefix;
  // A_Function_Call =>                           // 4.1
  // An_Indexed_Component (Is_Generalized_Indexing == true) //ASIS 2012 // 4.1.1
  Declaration_ID        Corresponding_Called_Function;
  // A_Function_Call =>                           // 4.1
  bool                  Is_Prefix_Call;
  Element_List          Function_Call_Parameters;
  // An_Indexed_Component =>                      // 4.1.1
  Expression_List       Index_Expressions;
  bool                  Is_Generalized_Indexing;
  // A_Slice =>                                   // 4.1.2
  Discrete_Range_ID     Slice_Range;
  // A_Selected_Component =>                      // 4.1.3
  Expression_ID         Selector;
  // An_Attribute_Reference :
  enum Attribute_Kinds  atribute_kind;
  Expression_ID         Attribute_Designator_Identifier;
  Expression_List       Attribute_Designator_Expressions;
  // A_Record_Aggregate =>                        // 4.3
  // An_Extension_Aggregate =>                    // 4.3
  Association_List      Record_Component_Associations;
  // An_Extension_Aggregate =>                    // 4.3
  Expression_ID         Extension_Aggregate_Expression;
  // A_Positional_Array_Aggregate |               // 4.3
  // A_Named_Array_Aggregate =>                   // 4.3
  Association_List      Array_Component_Associations;
  // An_And_Then_Short_Circuit |                  // 4.4
  // An_Or_Else_Short_Circuit =>                  // 4.4
  Expression_ID         Short_Circuit_Operation_Left_Expression;
  Expression_ID         Short_Circuit_Operation_Right_Expression;
  // An_In_Membership_Test |                      // 4.4  Ada 2012
  // A_Not_In_Membership_Test =>                  // 4.4  Ada 2012
  Expression_ID         Membership_Test_Expression;
  Element_List          Membership_Test_Choices;
  // A_Parenthesized_Expression =>                // 4.4
  Expression_ID         Expression_Parenthesized;
  // A_Type_Conversion =>                         // 4.6
  // A_Qualified_Expression =>                    // 4.7
  Expression_ID         Converted_Or_Qualified_Subtype_Mark;
  Expression_ID         Converted_Or_Qualified_Expression;
  Expression_ID         Predicate;
  // An_Allocation_From_Subtype =>                // 4.8
  // An_Allocation_From_Qualified_Expression =>   // 4.8
  Expression_ID         Subpool_Name;
  // An_Allocation_From_Subtype =>                // 4.8
  Subtype_Indication_ID Allocator_Subtype_Indication;
  // An_Allocation_From_Qualified_Expression =>   // 4.8
  Expression_ID         Allocator_Qualified_Expression;
  // A_Case_Expression |                          // Ada 2012
  // An_If_Expression =>                          // Ada 2012
  Expression_Path_List  Expression_Paths;
  // A_For_All_Quantified_Expression |            // Ada 2012
  // A_For_Some_Quantified_Expression =>          // Ada 2012
  Declaration_ID        Iterator_Specification;
  // An expression that references an entity declared within the
  // implicit specification of a generic instantiation:
  Defining_Name_ID      Corresponding_Generic_Element;
#endif

     Expression_Kinds expressionKind = expression.Expression_Kind;

     printf ("   In processExpression(): \n");
     printf ("      expressionKind (value) = %d \n",expressionKind);
     printf ("      expressionKind (name)  = %s \n",expressionKindName(expressionKind).c_str());

  // Moved from Declaration_Struct
  // An expression that references an entity declared within the implicit specification of a generic instantiation:
  // Defining_Name_ID      Corresponding_Generic_Element;
     printf ("      Corresponding_Generic_Element = %d \n",expression.Corresponding_Generic_Element);

     switch (expressionKind)
        {

       // An_Integer_Literal,                        // 2.4
       // A_Real_Literal,                            // 2.4.1
       // A_String_Literal,                          // 2.6
          case An_Integer_Literal:
          case A_Real_Literal:
          case A_String_Literal:
             {
               char* valueImage = expression.Value_Image;
               printf ("     valueImage = %s \n",valueImage);

               SgUntypedExpression* untypedExpression = NULL;

               switch (expressionKind)
                  {
                    case An_Integer_Literal:
                       {
                      // This is a reference to an enum
                         untypedExpression = new SgUntypedExpression(e_integerLiteral);
                         break;
                       }

                    case A_Real_Literal:
                       {
                      // This is a reference to an enum
                         untypedExpression = new SgUntypedExpression(e_realLiteral);
                         break;
                       }

                    case A_String_Literal:
                       {
                      // This is a reference to an enum
                         untypedExpression = new SgUntypedExpression(e_stringLiteral);
                         break;
                       }

                    default:
                       {
                         printf ("default reached in processStatement(): collected cases: \n");
                         ROSE_ASSERT(false);
                       }
                  }

               ROSE_ASSERT(untypedExpression != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               processUntypedNode(untypedExpression,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Identifier |                              // 4.1
       // An_Operator_Symbol |                         // 4.1
       // A_Character_Literal |                        // 4.1
       // An_Enumeration_Literal:
          case An_Identifier:
       // case An_Operator_Symbol:
          case A_Character_Literal:
          case An_Enumeration_Literal:
             {
               char* nameImage                                      = expression.Name_Image;
               printf ("     nameImage = %s \n",nameImage);

               Defining_Name_ID correspondingNameDefinition         = expression.Corresponding_Name_Definition;
               printf ("     correspondingNameDefinition = %d \n",correspondingNameDefinition);

            // Only >1 if the expression in a pragma is ambiguous
               Defining_Name_List & correspondingNameDefinitionList = expression.Corresponding_Name_Definition_List;
               printf ("     correspondingNameDefinitionList: \n");
               processDefiningNameList(correspondingNameDefinitionList);

               Element_ID correspondingNameDeclaration            = expression.Corresponding_Name_Declaration; // Decl or stmt
               printf ("     correspondingNameDeclaration = %d \n",correspondingNameDeclaration);

               SgUntypedExpression* untypedExpression = NULL;

               switch (expressionKind)
                  {
                    case An_Identifier:
                       {
                      // This is a reference to an enum
                         untypedExpression = new SgUntypedExpression(e_identifier);
                         break;
                       }

                    case A_Character_Literal:
                       {
                      // This is a reference to an enum
                         untypedExpression = new SgUntypedExpression(e_characterLiteral);
                         break;
                       }

                    case An_Enumeration_Literal:
                       {
                      // This is a reference to an enum
                         untypedExpression = new SgUntypedExpression(e_enumerationLiteral);
                         break;
                       }

                    default:
                       {
                         printf ("default reached in processStatement(): collected cases: \n");
                         ROSE_ASSERT(false);
                       }
                  }

               ROSE_ASSERT(untypedExpression != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               if (untypedNodeMap.find(element_id) == untypedNodeMap.end())
                  {
                    processUntypedNode(untypedExpression,element_id);
                  }
                 else
                  {
                    printf ("Skipping redundent processing of element_id = %d \n",element_id);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Operator_Symbol:
          case An_Operator_Symbol:
             {
               char* nameImage                                      = expression.Name_Image;
               printf ("     nameImage = %s \n",nameImage);

               Defining_Name_ID correspondingNameDefinition         = expression.Corresponding_Name_Definition;
               printf ("     correspondingNameDefinition = %d \n",correspondingNameDefinition);

            // Only >1 if the expression in a pragma is ambiguous
               Defining_Name_List & correspondingNameDefinitionList = expression.Corresponding_Name_Definition_List;
               printf ("     correspondingNameDefinitionList: \n");
               processDefiningNameList(correspondingNameDefinitionList);

               Element_ID correspondingNameDeclaration            = expression.Corresponding_Name_Declaration; // Decl or stmt
               printf ("     correspondingNameDeclaration = %d \n",correspondingNameDeclaration);

               Operator_Kinds operatorKind = expression.Operator_Kind;
               printf ("     operatorKind (value) = %d \n",operatorKind);
               printf ("     operatorKind (name)  = %s \n",operatorKindName(operatorKind).c_str());

               ExpressionKind operatorEnum = e_unknown;
               switch (operatorKind)
                  {
                    case Not_An_Operator:                  operatorEnum = e_notAnOperator;              break;
                    case An_And_Operator:                  operatorEnum = e_andOperator;                break;
                    case An_Or_Operator:                   operatorEnum = e_orOperator;                 break;
                    case An_Xor_Operator:                  operatorEnum = e_xorOperator;                break;
                    case An_Equal_Operator:                operatorEnum = e_equalOperator;              break;
                    case A_Not_Equal_Operator:             operatorEnum = e_notEqualOperator;           break;
                    case A_Less_Than_Operator:             operatorEnum = e_lessThanOperator;           break;
                    case A_Less_Than_Or_Equal_Operator:    operatorEnum = e_lessThanOrEqualOperator;    break;
                    case A_Greater_Than_Operator:          operatorEnum = e_greaterThanOperator;        break;
                    case A_Greater_Than_Or_Equal_Operator: operatorEnum = e_greaterThanOrEqualOperator; break;
                    case A_Plus_Operator:                  operatorEnum = e_plusOperator;               break;
                    case A_Minus_Operator:                 operatorEnum = e_minusOperator;              break;
                    case A_Concatenate_Operator:           operatorEnum = e_concatenateOperator;        break;
                    case A_Unary_Plus_Operator:            operatorEnum = e_unaryPlusOperator;          break;
                    case A_Unary_Minus_Operator:           operatorEnum = e_unaryMinusOperator;         break;
                    case A_Multiply_Operator:              operatorEnum = e_multiplyOperator;           break;
                    case A_Divide_Operator:                operatorEnum = e_divideOperator;             break;
                    case A_Mod_Operator:                   operatorEnum = e_modOperator;                break;
                    case A_Rem_Operator:                   operatorEnum = e_remOperator;                break;
                    case An_Exponentiate_Operator:         operatorEnum = e_exponentiateOperator;       break;
                    case An_Abs_Operator:                  operatorEnum = e_absOperator;                break;
                    case A_Not_Operator:                   operatorEnum = e_notOperator;                break;

                    default:
                       {
                         printf ("Error: default called in switch for Ada_ROSE_Translation::processExpression(): operatorKind = %d = %s \n",operatorKind,operatorKindName(operatorKind).c_str());
                         ROSE_ASSERT(false);
                       }
                  }

               SgUntypedExpression* untypedExpression = new SgUntypedExpression(operatorEnum);
               ROSE_ASSERT(untypedExpression != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               processUntypedNode(untypedExpression,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Explicit_Dereference =>                   // 4.1
       // A_Function_Call =>                           // 4.1
       // An_Indexed_Component =>                      // 4.1.1
       // A_Slice =>                                   // 4.1.2
       // A_Selected_Component =>                      // 4.1.3
       // An_Attribute_Reference =>                    // 4.1.4
          case An_Explicit_Dereference:
       // case A_Function_Call:
       // case An_Indexed_Component:
       // case A_Slice:
       // case A_Selected_Component:
       // case An_Attribute_Reference:
             {
               Expression_ID prefix = expression.Prefix;
               printf ("     prefix = %d \n",prefix);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Selected_Component =>                      // 4.1.3
          case A_Selected_Component:
             {
               Expression_ID prefix = expression.Prefix;
               printf ("     prefix = %d \n",prefix);
               Expression_ID selector = expression.Selector;
               printf ("     selector = %d \n",selector);

               SgUntypedExpression* untypedExpression = new SgUntypedExpression(e_selectedComponent);
               ROSE_ASSERT(untypedExpression != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               processUntypedNode(untypedExpression,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Function_Call =>                           // 4.1
       // An_Indexed_Component (Is_Generalized_Indexing == true) //ASIS 2012 // 4.1.1
       // case A_Function_Call:
          case An_Indexed_Component:
             {
               Expression_ID prefix = expression.Prefix;
               printf ("     prefix = %d \n",prefix);

               Declaration_ID correspondingCalledFunction = expression.Corresponding_Called_Function;
               printf ("     correspondingCalledFunction = %d \n",correspondingCalledFunction);

               Expression_List & indexExpressions        = expression.Index_Expressions;
               printf ("     indexExpressions: \n");
               processElementList(indexExpressions);
               bool              isGeneralizedIndexing = expression.Is_Generalized_Indexing;
               printf ("     isGeneralizedIndexing = %s \n",isGeneralizedIndexing ? "true" : "false");

#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Function_Call =>                           // 4.1
          case A_Function_Call:
             {
               Expression_ID prefix = expression.Prefix;
               printf ("     prefix = %d \n",prefix);

               Declaration_ID correspondingCalledFunction = expression.Corresponding_Called_Function;
               printf ("     correspondingCalledFunction = %d \n",correspondingCalledFunction);

               bool isPrefixCall = expression.Is_Prefix_Call;
               printf ("     isPrefixCall = %s \n",isPrefixCall ? "true" : "false");
               Element_ID_List & functionCallParameters = expression.Function_Call_Parameters;
               printf ("     functionCallParameters: \n");
               processElementList(functionCallParameters);

               SgUntypedExpression* untypedExpression = new SgUntypedExpression(e_functionCall);
               ROSE_ASSERT(untypedExpression != NULL);

            // ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               if (untypedNodeMap.find(element_id) == untypedNodeMap.end())
                  {
                    processUntypedNode(untypedExpression,element_id);
                  }
                 else
                  {
                    printf ("Skipping redundent processing (case A_Function_Call) of element_id = %d \n",element_id);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Slice =>                                   // 4.1.2
          case A_Slice:
             {
               Expression_ID prefix = expression.Prefix;
               printf ("     prefix = %d \n",prefix);
               Discrete_Range_ID sliceRange = expression.Slice_Range;
               printf ("     sliceRange = %d \n",sliceRange);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Attribute_Reference :
          case An_Attribute_Reference:
             {
               Expression_ID prefix = expression.Prefix;
               printf ("     prefix = %d \n",prefix);

            // Attribute_Kinds atributeKind = expression.atribute_kind;
               Attribute_Kinds attributeKind = expression.Attribute_Kind;
               printf ("     attributeKind (value) = %d \n",attributeKind);
               printf ("     attributeKind (name)  = %s \n",attributeKindName(attributeKind).c_str());
               Expression_ID attributeDesignatorIdentifier = expression.Attribute_Designator_Identifier;
               printf ("     attributeDesignatorIdentifier = %d \n",attributeDesignatorIdentifier);
               Expression_List & attributeDesignatorExpressions = expression.Attribute_Designator_Expressions;
               printf ("     attributeDesignatorExpressions: \n");
               processExpressionList(attributeDesignatorExpressions);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Record_Aggregate =>                        // 4.3
       // An_Extension_Aggregate =>                    // 4.3
          case A_Record_Aggregate:
       // case An_Extension_Aggregate:
             {
               Association_List & recordComponentAssociations = expression.Record_Component_Associations;
               printf ("     recordComponentAssociations: \n");
               processAssociationList(recordComponentAssociations);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Extension_Aggregate =>                    // 4.3
          case An_Extension_Aggregate:
             {
               Association_List & recordComponentAssociations = expression.Record_Component_Associations;
               printf ("     recordComponentAssociations: \n");
               processAssociationList(recordComponentAssociations);

               Expression_ID extensionAggregateExpression = expression.Extension_Aggregate_Expression;
               printf ("     extensionAggregateExpression = %d \n",extensionAggregateExpression);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Positional_Array_Aggregate |               // 4.3
       // A_Named_Array_Aggregate =>                   // 4.3
          case A_Positional_Array_Aggregate:
          case A_Named_Array_Aggregate:
             {
               Association_List & arrayComponentAssociations = expression.Array_Component_Associations;
               printf ("     arrayComponentAssociations: \n");
               processAssociationList(arrayComponentAssociations);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_And_Then_Short_Circuit |                  // 4.4
       // An_Or_Else_Short_Circuit =>                  // 4.4
          case An_And_Then_Short_Circuit:
          case An_Or_Else_Short_Circuit:
             {
               Expression_ID shortCircuitOperationLeftExpression = expression.Short_Circuit_Operation_Left_Expression;
               printf ("     shortCircuitOperationLeftExpression = %d \n",shortCircuitOperationLeftExpression);
               Expression_ID shortCircuitOperationRightExpression = expression.Short_Circuit_Operation_Right_Expression;
               printf ("     shortCircuitOperationRightExpression = %d \n",shortCircuitOperationRightExpression);
#if 1
               printf ("Exiting as a test! \n");
               //~ ROSE_ASSERT(false);
#endif
               break;
             }

       // An_In_Membership_Test |                      // 4.4  Ada 2012
       // A_Not_In_Membership_Test =>                  // 4.4  Ada 2012
          case An_In_Membership_Test:
          case A_Not_In_Membership_Test:
             {
               Expression_ID membershipTestExpression = expression.Membership_Test_Expression;
               printf ("     membershipTestExpression = %d \n",membershipTestExpression);
               Element_ID_List & membershipTestChoices = expression.Membership_Test_Choices;
               printf ("     membershipTestChoices: \n");
               processExpressionList(membershipTestChoices);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }


       // A_Parenthesized_Expression =>                // 4.4
          case A_Parenthesized_Expression:
             {
               Expression_ID expressionParenthesized = expression.Expression_Parenthesized;
               printf ("     expressionParenthesized = %d \n",expressionParenthesized);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }


       // A_Type_Conversion =>                         // 4.6
       // A_Qualified_Expression =>                    // 4.7
          case A_Type_Conversion:
          case A_Qualified_Expression:
             {
               Expression_ID convertedOrQualifiedSubtypeMark = expression.Converted_Or_Qualified_Subtype_Mark;
               printf ("     convertedOrQualifiedSubtypeMark = %d \n",convertedOrQualifiedSubtypeMark);
               Expression_ID convertedOrQualifiedExpression = expression.Converted_Or_Qualified_Expression;
               printf ("     convertedOrQualifiedExpression = %d \n",convertedOrQualifiedExpression);
               Expression_ID predicate = expression.Predicate;
               printf ("     predicate = %d \n",predicate);

               SgUntypedExpression* untypedExpression = NULL;
               if (expressionKind == A_Type_Conversion)
                  {
                    untypedExpression = new SgUntypedExpression(e_typeConversion);
                  }
                 else
                  {
                    untypedExpression = new SgUntypedExpression(e_qualifiedExpression);
                  }

               ROSE_ASSERT(untypedExpression != NULL);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               processUntypedNode(untypedExpression,element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // An_Allocation_From_Subtype =>                // 4.8
       // An_Allocation_From_Qualified_Expression =>   // 4.8
       // case An_Allocation_From_Subtype:
       // case An_Allocation_From_Qualified_Expression:
             {
               Expression_ID subpoolName = expression.Subpool_Name;
               printf ("     subpoolName = %d \n",subpoolName);
               break;
             }
#endif

       // An_Allocation_From_Subtype =>                // 4.8
          case An_Allocation_From_Subtype:
             {
               Expression_ID subpoolName = expression.Subpool_Name;
               printf ("     subpoolName = %d \n",subpoolName);

               Subtype_Indication_ID allocatorSubtypeIndication = expression.Allocator_Subtype_Indication;
               printf ("     allocatorSubtypeIndication = %d \n",allocatorSubtypeIndication);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // An_Allocation_From_Qualified_Expression =>   // 4.8
          case An_Allocation_From_Qualified_Expression:
             {
               Expression_ID subpoolName = expression.Subpool_Name;
               printf ("     subpoolName = %d \n",subpoolName);

               Expression_ID allocatorQualifiedExpression = expression.Allocator_Qualified_Expression;
               printf ("     allocatorQualifiedExpression = %d \n",allocatorQualifiedExpression);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Case_Expression|                          // Ada 2012
       // An_If_Expression =>                          // Ada 2012
          case A_Case_Expression:
          case An_If_Expression:
             {
               Expression_Path_List & expressionPaths = expression.Expression_Paths;
               printf ("     expressionPaths: \n");
               processPathList(expressionPaths);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_For_All_Quantified_Expression |            // Ada 2012
       // A_For_Some_Quantified_Expression =>          // Ada 2012
          case A_For_All_Quantified_Expression:
          case A_For_Some_Quantified_Expression:
             {
               Declaration_ID iteratorSpecification = expression.Iterator_Specification;
               printf ("     iteratorSpecification = %d \n",iteratorSpecification);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               printf ("Default reached in processDeclaration(): declarationKind = %d declarationKind = %s \n",expressionKind,expressionKindName(expressionKind).c_str());
               ROSE_ASSERT(false);
             }
        }
   }


void
  Ada_ROSE_Translation::processDefinition( Definition_Struct & definition, int element_id)
   {
#if 0
  // Documentation for Definition_Struct.
     enum Definition_Kinds Definition_Kind;
     Definition_Union      The_Union;

  // Documentation for Definition_Kinds
     Not_A_Definition,                 // An unexpected element
     A_Type_Definition,                // 3.2.1(4)    -> Type_Kinds
     A_Subtype_Indication,             // 3.2.2(3)
     A_Constraint,                     // 3.2.2(5)    -> Constraint_Kinds
     A_Component_Definition,           // 3.6(7)      -> Trait_Kinds
     A_Discrete_Subtype_Definition,    // 3.6(6)      -> Discrete_Range_Kinds
     A_Discrete_Range,                 // 3.6.1(3)    -> Discrete_Range_Kinds
     An_Unknown_Discriminant_Part,     // 3.7(3)
     A_Known_Discriminant_Part,        // 3.7(2)
     A_Record_Definition,              // 3.8(3)
     A_Null_Record_Definition,         // 3.8(3)
     A_Null_Component,                 // 3.8(4)
     A_Variant_Part,                   // 3.8.1(2)
     A_Variant,                        // 3.8.1(3)
     An_Others_Choice,                 // 3.8.1(5), 4.3.1(5), 4.3.3(5), 11.2(5)

  //  //|A2005 start
     An_Access_Definition,             // 3.10(6/2)   -> Access_Definition_Kinds
  //  //|A2005 end

     A_Private_Type_Definition,        // 7.3(2)      -> Trait_Kinds
     A_Tagged_Private_Type_Definition, // 7.3(2)      -> Trait_Kinds
     A_Private_Extension_Definition,   // 7.3(3)      -> Trait_Kinds
     A_Task_Definition,                // 9.1(4)
     A_Protected_Definition,           // 9.4(4)
     A_Formal_Type_Definition,         // 12.5(3)     -> Formal_Type_Kinds

  //  //|A2012 start
     An_Aspect_Specification           // 13.3.1
  //  //|A2012 end


  // Documentation for Definition_Union
typedef union _Definition_Union {
  int                                   Dummy_Member; // For Ada default initialization
  Type_Definition_Struct                The_Type_Definition;
  Subtype_Indication_Struct             The_Subtype_Indication;
  Constraint_Struct                     The_Constraint;
  Component_Definition_Struct           The_Component_Definition;
  Discrete_Subtype_Definition_Struct    The_Discrete_Subtype_Definition;
  Discrete_Range_Struct                 The_Discrete_Range;
  Unknown_Discriminant_Part_Struct      The_Unknown_Discriminant_Part;
  Known_Discriminant_Part_Struct        The_Known_Discriminant_Part;
  Record_Definition_Struct              The_Record_Definition;
  Null_Record_Definition_Struct         The_Null_Record_Definition;
  Null_Component_Struct                 The_Null_Component;
  Variant_Part_Struct                   The_Variant_Part;
  Variant_Struct                        The_Variant;
  Others_Choice_Struct                  The_Others_Choice;
  Access_Definition_Struct              The_Access_Definition;
  Private_Type_Definition_Struct        The_Private_Type_Definition;
  Tagged_Private_Type_Definition_Struct The_Tagged_Private_Type_Definition;
  Private_Extension_Definition_Struct   The_Private_Extension_Definition;
  Task_Definition_Struct                The_Task_Definition;
  Protected_Definition_Struct           The_Protected_Definition;
  Formal_Type_Definition_Struct         The_Formal_Type_Definition;
  Aspect_Specification_Struct           The_Aspect_Specification;
} Definition_Union;


#endif

     Definition_Kinds definitionKind = definition.Definition_Kind;

     printf ("   In processDefinition(): \n");
     printf ("      definitionKind (value) = %d \n",definitionKind);
     printf ("      definitionKind (name)  = %s \n",definitionKindName(definitionKind).c_str());

  // By default we always build a SgUntypeScope, until we get what we build to be more finely tailored to what we need.
     bool buildDefaultUntypedNode = true;

     switch (definitionKind)
        {
          case Not_A_Definition:
             {
               printf ("Error: switch has case Not_A_Definition \n");
               ROSE_ASSERT(false);
               break;
             }

          case A_Type_Definition:
             {
               processTypeDefinition(definition.The_Union.The_Type_Definition,element_id,buildDefaultUntypedNode);
               break;
             }

          case A_Subtype_Indication:
             {
               processSubtypeIndication(definition.The_Union.The_Subtype_Indication,element_id);
               break;
             }

          case A_Constraint:
             {
               processConstraint(definition.The_Union.The_Constraint,element_id);
               break;
             }

          case A_Component_Definition:
             {
               processComponentDefinition(definition.The_Union.The_Component_Definition,element_id);
               break;
             }

          case A_Discrete_Subtype_Definition:
             {
               processDiscreteSubtypeDefinition(definition.The_Union.The_Discrete_Subtype_Definition,element_id);
               break;
             }

          case A_Discrete_Range:
             {
               processDiscreteRange(definition.The_Union.The_Discrete_Range,element_id);
               break;
             }

          case An_Unknown_Discriminant_Part:
             {
               processUnknownDiscriminantPart(definition.The_Union.The_Unknown_Discriminant_Part,element_id);
               break;
             }

          case A_Known_Discriminant_Part:
             {
               processKnownDiscriminantPart(definition.The_Union.The_Known_Discriminant_Part,element_id);
               break;
             }

          case A_Record_Definition:
             {
               processRecordDefinition(definition.The_Union.The_Record_Definition,element_id);

               buildDefaultUntypedNode = false;

               break;
             }

          case A_Null_Record_Definition:
             {
               processNullRecordDefinition(definition.The_Union.The_Null_Record_Definition,element_id);
               break;
             }

          case A_Null_Component:
             {
               processNullComponent(definition.The_Union.The_Null_Component,element_id);
               break;
             }

          case A_Variant_Part:
             {
               processVariantPart(definition.The_Union.The_Variant_Part,element_id);
               break;
             }

          case A_Variant:
             {
               processVariant(definition.The_Union.The_Variant,element_id);
               break;
             }

          case An_Others_Choice:
             {
               processOthersChoice(definition.The_Union.The_Others_Choice,element_id);
               break;
             }

          case An_Access_Definition:
             {
               processAccessDefinition(definition.The_Union.The_Access_Definition,element_id);
               break;
             }

          case A_Private_Type_Definition:
             {
               processPrivateTypeDefinition(definition.The_Union.The_Private_Type_Definition,element_id);
               break;
             }

          case A_Tagged_Private_Type_Definition:
             {
               processTaggedPrivateTypeDefinition(definition.The_Union.The_Tagged_Private_Type_Definition,element_id);
               break;
             }

          case A_Private_Extension_Definition:
             {
               processPrivateExtensionDefinition(definition.The_Union.The_Private_Extension_Definition,element_id);
               break;
             }

          case A_Task_Definition:
             {
               processTaskDefinition(definition.The_Union.The_Task_Definition,element_id);
               break;
             }

          case A_Protected_Definition:
             {
               processProtectedDefinition(definition.The_Union.The_Protected_Definition,element_id);
               break;
             }

          case A_Formal_Type_Definition:
             {
               processFormalTypeDefinition(definition.The_Union.The_Formal_Type_Definition,element_id);
               break;
             }

          case An_Aspect_Specification:
             {
               processAspectSpecification(definition.The_Union.The_Aspect_Specification,element_id);
               break;
             }

          default:
             {
               printf ("Default reached in processDefinition(): definitionKind = %d definitionKind = %s element_id = %d \n",definitionKind,definitionKindName(definitionKind).c_str(),element_id);
               ROSE_ASSERT(false);
             }
        }


     if (buildDefaultUntypedNode == true)
        {
       // DQ (10/5/2017): This is general code to handled definitions, we likely new additional
       // untyped IR nodes to support the concept fo a SgUntypedDefinition, plus whatever
       // variations of untyped definition IR nodes that we require (for the cases above).
          SgUntypedScope* untypedScope = new SgUntypedScope();
          ROSE_ASSERT(untypedScope != NULL);

          SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
          ROSE_ASSERT(untypedStatementList != NULL);

       // Connect IR nodes.
          untypedScope->set_statement_list(untypedStatementList);

          printf ("In processDefinition(): element_id = %d \n",element_id);

       // ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
          if (untypedNodeMap.find(element_id) == untypedNodeMap.end())
             {
               untypedNodeMap[element_id] = untypedScope;
             }
            else
             {
               printf ("ERROR: ignoring redundent processing of definition: definitionKind = %d definitionKind = %s \n",definitionKind,definitionKindName(definitionKind).c_str());
             }
          ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

          setAsisAttribute (untypedScope,element_id);
        }

   }


void
Ada_ROSE_Translation::processTypeDefinition ( Type_Definition_Struct & x, int element_id, bool & buildDefaultUntypedNode )
   {
#if 0
  // Documentation for Type_Definition_Struct
     Type_Kinds           Type_Kind;
     bool                 Has_Abstract;
     bool                 Has_Limited;
     bool                 Has_Private;
     Declaration_List     Corresponding_Type_Operators;
  // These fields are only valid for the kinds above them:
  // An_Interface_Type_Definition
     bool                 Has_Protected;
     bool                 Has_Synchronized;
  // A_Tagged_Record_Type_Definition
     bool                 Has_Tagged;
  // A_Task_Definition
     bool                 Has_Task;
  // A_Discriminant_Specification
  // A_Parameter_Specification
  // A_Formal_Object_Declaration
  // An_Object_Renaming_Declaration
     bool                 Has_Null_Exclusion;
  // An_Interface_Type_Definition
     Interface_Kinds      Interface_Kind;
  // A_Root_Type_Definition
     Root_Type_Kinds      Root_Type_Kind;
  // A_Derived_Type_Definition
  // A_Derived_Record_Extension_Definition
     Subtype_Indication   Parent_Subtype_Indication;

  // A_Derived_Record_Extension_Definition
  // A_Record_Type_Definition
  // A_Tagged_Record_Type_Definition
     Definition           Record_Definition;
  // A_Derived_Type_Definition
  // A_Derived_Record_Extension_Definition
     Declaration_List     Implicit_Inherited_Declarations;
     Declaration_List     Implicit_Inherited_Subprograms;
     Declaration          Corresponding_Parent_Subtype;
     Declaration          Corresponding_Root_Type;
     Declaration          Corresponding_Type_Structure;
  // An_Enumeration_Type_Definition
     Declaration_List     Enumeration_Literal_Declarations;
  // A_Signed_Integer_Type_Definition
     Range_Constraint     Integer_Constraint;

  // A_Modular_Type_Definition
     Expression           Mod_Static_Expression;
  // A_Floating_Point_Definition
  // A_Decimal_Fixed_Point_Definition
     Expression           Digits_Expression;
  // An_Ordinary_Fixed_Point_Definition
  // A_Decimal_Fixed_Point_Definition
     Expression           Delta_Expression;
  // A_Floating_Point_Definition
  // An_Ordinary_Fixed_Point_Definition
  // A_Decimal_Fixed_Point_Definition
     Range_Constraint     Real_Range_Constraint;
  // An_Unconstrained_Array_Definition
     Expression_List      Index_Subtype_Definitions;
  // A_Constrained_Array_Definition
     Expression_List      Discrete_Subtype_Definitions;
  // An_Unconstrained_Array_Definition
  // A_Constrained_Array_Definition
     Component_Definition Array_Component_Definition;
  // A_Derived_Record_Extension_Definition
  // An_Interface_Type_Definition
     Expression_List      Definition_Interface_List;
  // An_Access_Type_Definition
     Access_Type_Struct   Access_Type;

  // Documentation ofr Type_Kinds
     Not_A_Type_Definition,                 // An unexpected element
     A_Derived_Type_Definition,             // 3.4(2)     -> Trait_Kinds
     A_Derived_Record_Extension_Definition, // 3.4(2)     -> Trait_Kinds
     An_Enumeration_Type_Definition,        // 3.5.1(2)
     A_Signed_Integer_Type_Definition,      // 3.5.4(3)
     A_Modular_Type_Definition,             // 3.5.4(4)
     A_Root_Type_Definition,                // 3.5.4(14), 3.5.6(3) -> Root_Type_Kinds
     A_Floating_Point_Definition,           // 3.5.7(2)
     An_Ordinary_Fixed_Point_Definition,    // 3.5.9(3)
     A_Decimal_Fixed_Point_Definition,      // 3.5.9(6)
     An_Unconstrained_Array_Definition,     // 3.6(2)
     A_Constrained_Array_Definition,        // 3.6(2)
     A_Record_Type_Definition,              // 3.8(2)     -> Trait_Kinds
     A_Tagged_Record_Type_Definition,       // 3.8(2)     -> Trait_Kinds
     An_Interface_Type_Definition,          // 3.9.4      -> Interface_Kinds
     An_Access_Type_Definition              // 3.10(2)    -> Access_Type_Kinds
#endif

     Type_Kinds typeKind = x.Type_Kind;

     printf ("   In processTypeDefinition(): \n");
     printf ("      typeKind (value) = %d \n",typeKind);
     printf ("      typeKind (name)  = %s \n",typeKindName(typeKind).c_str());

     bool hasAbstract = x.Has_Abstract;
     printf ("      hasAbstract = %s \n",hasAbstract ? "true" : "false");

     bool hasLimited = x.Has_Limited;
     printf ("      hasLimited = %s \n",hasLimited ? "true" : "false");

     bool hasPrivate = x.Has_Private;
     printf ("      hasPrivate = %s \n",hasPrivate ? "true" : "false");

     Declaration_List & correspondingTypeOperators = x.Corresponding_Type_Operators;
     printf ("   correspondingTypeOperators:  \n");
     processDeclarationList(correspondingTypeOperators);

     switch (typeKind)
        {
          case Not_A_Type_Definition:
             {
               printf ("ERROR: case Not_A_Type_Definition \n");
               ROSE_ASSERT(false);
               break;
             }
#if 0
       // DQ (10/12/2017): I think this is not a valid enum value.
          case A_Task_Definition:
             {
               bool hasTask = x.Has_Task;
               printf ("   hasTask = %s \n",hasTask ? "true" : "false");
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }
#endif
#if 0
       // DQ (10/12/2017): I think these are not valid enum values.
       // A_Discriminant_Specification
       // A_Parameter_Specification
       // A_Formal_Object_Declaration
       // An_Object_Renaming_Declaration
          case A_Discriminant_Specification:
          case A_Parameter_Specification:
          case A_Formal_Object_Declaration:
          case An_Object_Renaming_Declaration:
             {
               bool hasNullExclusion = x.Has_Null_Exclusion;
               printf ("   hasNullExclusion = %s \n",hasNullExclusion ? "true" : "false");
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }
#endif

          case A_Derived_Type_Definition:
             {
            // Subtype_Indication & parentSubtypeIndication = x.Parent_Subtype_Indication;
            // processElement(parentSubtypeIndication);
               Subtype_Indication parentSubtypeIndication = x.Parent_Subtype_Indication;
               printf ("   parentSubtypeIndication = %d \n",parentSubtypeIndication);

               Declaration_List & implicitInheritedDeclarations = x.Implicit_Inherited_Declarations;
               processDeclarationList(implicitInheritedDeclarations);
               Declaration_List & implicitInheritedSubprograms = x.Implicit_Inherited_Subprograms;
               processDeclarationList(implicitInheritedSubprograms);
               Declaration correspondingParentSubtype = x.Corresponding_Parent_Subtype;
               printf ("   correspondingParentSubtype = %d \n",correspondingParentSubtype);
               Declaration correspondingRootType = x.Corresponding_Root_Type;
               printf ("   correspondingRootType = %d \n",correspondingRootType);
               Declaration correspondingTypeStructure = x.Corresponding_Type_Structure;
               printf ("   correspondingTypeStructure = %d \n",correspondingTypeStructure);

#if 0
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Derived_Record_Extension_Definition:
             {
               Subtype_Indication parentSubtypeIndication = x.Parent_Subtype_Indication;
               printf ("   parentSubtypeIndication = %d \n",parentSubtypeIndication);

               Definition recordDefinition = x.Record_Definition;
               printf ("   recordDefinition = %d \n",recordDefinition);

               Declaration_List & implicitInheritedDeclarations = x.Implicit_Inherited_Declarations;
               processDeclarationList(implicitInheritedDeclarations);
               Declaration_List & implicitInheritedSubprograms = x.Implicit_Inherited_Subprograms;
               processDeclarationList(implicitInheritedSubprograms);
               Declaration correspondingParentSubtype = x.Corresponding_Parent_Subtype;
               printf ("   correspondingParentSubtype = %d \n",correspondingParentSubtype);
               Declaration correspondingRootType = x.Corresponding_Root_Type;
               printf ("   correspondingRootType = %d \n",correspondingRootType);
               Declaration correspondingTypeStructure = x.Corresponding_Type_Structure;
               printf ("   correspondingTypeStructure = %d \n",correspondingTypeStructure);

               Expression_List & definitionInterfaceList = x.Definition_Interface_List;
               processExpressionList(definitionInterfaceList);
#if 0
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Enumeration_Type_Definition:
             {
               Declaration_List & enumerationLiteralDeclarations = x.Enumeration_Literal_Declarations;
               processDeclarationList(enumerationLiteralDeclarations);
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Signed_Integer_Type_Definition:
             {
               Range_Constraint integerConstraint = x.Integer_Constraint;
               printf ("   integerConstraint = %d \n",integerConstraint);
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Modular_Type_Definition:
             {
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Root_Type_Definition:
             {
               Root_Type_Kinds rootTypeKind = x.Root_Type_Kind;
               printf ("rootTypeKind (value) = %d \n",rootTypeKind);
               printf ("rootTypeKind (name)  = %s \n",rootTypeKindName(rootTypeKind).c_str());
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Floating_Point_Definition:
             {
               Expression digitsExpression = x.Digits_Expression;
               printf ("   digitsExpression = %d \n",digitsExpression);

               Range_Constraint realRangeConstraint = x.Real_Range_Constraint;
               printf ("   realRangeConstraint = %d \n",realRangeConstraint);
#if 1
               printf ("Not implemented! \n");
               //~ ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Ordinary_Fixed_Point_Definition:
             {
               Expression deltaExpression = x.Delta_Expression;
               printf ("   deltaExpression = %d \n",deltaExpression);

               Range_Constraint realRangeConstraint = x.Real_Range_Constraint;
               printf ("   realRangeConstraint = %d \n",realRangeConstraint);
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Decimal_Fixed_Point_Definition:
             {
               Expression digitsExpression = x.Digits_Expression;
               printf ("   digitsExpression = %d \n",digitsExpression);

               Expression deltaExpression = x.Delta_Expression;
               printf ("   deltaExpression = %d \n",deltaExpression);

               Range_Constraint realRangeConstraint = x.Real_Range_Constraint;
               printf ("   realRangeConstraint = %d \n",realRangeConstraint);
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Unconstrained_Array_Definition:
             {
               Expression_List & indexSubtypeDefinitions = x.Index_Subtype_Definitions;
               processExpressionList(indexSubtypeDefinitions);

               Component_Definition arrayComponentDefinition = x.Array_Component_Definition;
               printf ("   arrayComponentDefinition = %d \n",arrayComponentDefinition);
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Constrained_Array_Definition:
             {
               Expression_List & discreteSubtypeDefinitions = x.Discrete_Subtype_Definitions;
               processExpressionList(discreteSubtypeDefinitions);

               Component_Definition arrayComponentDefinition = x.Array_Component_Definition;
               printf ("   arrayComponentDefinition = %d \n",arrayComponentDefinition);
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Record_Type_Definition:
             {
               Definition recordDefinition = x.Record_Definition;
               printf ("   recordDefinition = %d \n",recordDefinition);

               SgUntypedStructureDeclaration* untypedStructureDeclaration = new SgUntypedStructureDeclaration();
               ROSE_ASSERT(untypedStructureDeclaration != NULL);

               processUntypedNode(untypedStructureDeclaration,element_id);

               buildDefaultUntypedNode = false;
#if 0
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Tagged_Record_Type_Definition:
             {
               Definition recordDefinition = x.Record_Definition;
               printf ("   recordDefinition = %d \n",recordDefinition);

               bool hasTagged = x.Has_Tagged;
               printf ("hasTagged = %s \n",hasTagged ? "true" : "false");
#if 0
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Interface_Type_Definition:
             {
               Interface_Kinds interfaceKind = x.Interface_Kind;
               printf ("   interfaceKind (value) = %d \n",interfaceKind);
               printf ("   interfaceKind (name)  = %s \n",interfaceKindName(interfaceKind).c_str());

               bool hasProtected = x.Has_Protected;
               printf ("   hasProtected = %s \n",hasProtected ? "true" : "false");
               bool hasSynchronized = x.Has_Synchronized;
               printf ("   hasSynchronized = %s \n",hasSynchronized ? "true" : "false");

               Expression_List & definitionInterfaceList = x.Definition_Interface_List;
               processExpressionList(definitionInterfaceList);
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case An_Access_Type_Definition:
             {
               Access_Type_Struct & accessType = x.Access_Type;
               processAccessType(accessType);
#if 1
               printf ("Not implemented! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               printf ("Default reached in processTypeDefinition(): typeKind = %d typeKind = %s \n",typeKind,typeKindName(typeKind).c_str());
               ROSE_ASSERT(false);
             }
          }

#if 0
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }


// void Ada_ROSE_Translation::processAccessType ( Access_Type_Struct & x, int element_id)
void
Ada_ROSE_Translation::processAccessType ( Access_Type_Struct & x )
   {
#if 0
  // Documentation for x
  Access_Type_Kinds            Access_Type_Kind;
  bool                         Has_Null_Exclusion;
  // These fields are only valid for the kinds above them:
  // An_Access_To_Function
  // An_Access_To_Protected_Function
  bool                         Is_Not_Null_Return;
  // A_Pool_Specific_Access_To_Variable
  // An_Access_To_Variable
  // An_Access_To_Constant
  Subtype_Indication           Access_To_Object_Definition;
  // An_Access_To_Procedure
  // An_Access_To_Protected_Procedure
  // An_Access_To_Function
  // An_Access_To_Protected_Function
  Parameter_Specification_List Access_To_Subprogram_Parameter_Profile;
  // An_Access_To_Function
  // An_Access_To_Protected_Function
  Element                      Access_To_Function_Result_Profile;
#endif

     Access_Type_Kinds accessTypeKind = x.Access_Type_Kind;

     printf ("   In processAccessType(): \n");
     printf ("      accessTypeKind (value) = %d \n",accessTypeKind);
     printf ("      accessTypeKind (name)  = %s \n",accessTypeKindName(accessTypeKind).c_str());


     bool hasNullExclusion = x.Has_Null_Exclusion;
     printf ("   hasNullExclusion = %s \n",hasNullExclusion ? "true" : "false");

     switch (accessTypeKind)
        {
          case Not_An_Access_Type_Definition:
             {
               printf ("ERROR: case Not_An_Access_Type_Definition \n");
               ROSE_ASSERT(false);
               break;
             }

       // An_Access_To_Function
       // An_Access_To_Protected_Function
          case An_Access_To_Function:
          case An_Access_To_Protected_Function:
             {
               bool isNotNullReturn = x.Is_Not_Null_Return;
               printf ("   isNotNullReturn = %s \n",isNotNullReturn ? "true" : "false");

               Element_ID accessToFunctionResultProfile = x.Access_To_Function_Result_Profile;
               printf ("   accessToFunctionResultProfile = %d \n",accessToFunctionResultProfile);

               Parameter_Specification_List & accessToSubprogramParameterProfile = x.Access_To_Subprogram_Parameter_Profile;
               printf ("   accessToSubprogramParameterProfile: \n");
               processParameterSpecificationList(accessToSubprogramParameterProfile);
               break;
             }

       // A_Pool_Specific_Access_To_Variable
       // An_Access_To_Variable
       // An_Access_To_Constant
          case A_Pool_Specific_Access_To_Variable:
          case An_Access_To_Variable:
          case An_Access_To_Constant:
             {
               Subtype_Indication accessToObjectDefinition = x.Access_To_Object_Definition;
               printf ("   accessToObjectDefinition = %d \n",accessToObjectDefinition);
               break;
             }

       // An_Access_To_Procedure
       // An_Access_To_Protected_Procedure
       // An_Access_To_Function
       // An_Access_To_Protected_Function
          case An_Access_To_Procedure:
          case An_Access_To_Protected_Procedure:
             {
               Parameter_Specification_List & accessToSubprogramParameterProfile = x.Access_To_Subprogram_Parameter_Profile;
               printf ("   accessToSubprogramParameterProfile: \n");
               processParameterSpecificationList(accessToSubprogramParameterProfile);
               break;
             }

          default:
             {
               printf ("Default reached in processAccessType(): accessTypeKind = %d accessTypeKind = %s \n",accessTypeKind,accessTypeKindName(accessTypeKind).c_str());
               ROSE_ASSERT(false);
             }
          }

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processSubtypeIndication ( Subtype_Indication_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
     bool       Has_Null_Exclusion;
     Expression Subtype_Mark;
     Constraint Subtype_Constraint;
#endif

     bool hasNullExclusion = x.Has_Null_Exclusion;
     printf ("   hasNullExclusion = %s \n",hasNullExclusion ? "true" : "false");

     Expression subtypeMark = x.Subtype_Mark;
     printf ("   subtypeMark = %d \n",subtypeMark);
     Constraint subtypeConstraint = x.Subtype_Constraint;
     printf ("   subtypeConstraint = %d \n",subtypeConstraint);

#if 0
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processConstraint ( Constraint_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
  Constraint_Kinds              Constraint_Kind;
  // These fields are only valid for the kinds above them:
  // A_Digits_Constraint
  Expression                    Digits_Expression;
  // A_Delta_Constraint
  Expression                    Delta_Expression;
  // A_Digits_Constraint
  // A_Delta_Constraint
  Range_Constraint              Real_Range_Constraint;
  // A_Simple_Expression_Range
  Expression                    Lower_Bound;
  Expression                    Upper_Bound;
  // A_Range_Attribute_Reference
  Expression                    Range_Attribute;
  // An_Index_Constraint
  Discrete_Range_List           Discrete_Ranges;
  // A_Discriminant_Constraint
  Discriminant_Association_List Discriminant_Associations;
#endif

     Constraint_Kinds constraintKind = x.Constraint_Kind;
     printf ("   constraintKind (value) = %d \n",constraintKind);
     printf ("   constraintKind (name)  = %s \n",constraintKindName(constraintKind).c_str());

     switch (constraintKind)
        {
          case A_Digits_Constraint:
             {
               Expression digitsExpression = x.Digits_Expression;
               printf ("   digitsExpression = %d \n",digitsExpression);

               Range_Constraint realRangeConstraint = x.Real_Range_Constraint;
               printf ("   realRangeConstraint = %d \n",realRangeConstraint);
               break;
             }

          case A_Delta_Constraint:
             {
               Expression deltaExpression = x.Delta_Expression;
               printf ("   deltaExpression = %d \n",deltaExpression);

               Range_Constraint realRangeConstraint = x.Real_Range_Constraint;
               printf ("   realRangeConstraint = %d \n",realRangeConstraint);
               break;
             }

       // A_Simple_Expression_Range
          case A_Simple_Expression_Range:
             {
               Expression lowerBound = x.Lower_Bound;
               printf ("   lowerBound = %d \n",lowerBound);
               Expression upperBound = x.Upper_Bound;
               printf ("   upperBound = %d \n",upperBound);
               break;
             }

       // A_Range_Attribute_Reference
          case A_Range_Attribute_Reference:
             {
               Expression rangeAttribute = x.Range_Attribute;
               printf ("   rangeAttribute = %d \n",rangeAttribute);
               break;
             }

       // An_Index_Constraint
          case An_Index_Constraint:
             {
               Discrete_Range_List & discreteRanges = x.Discrete_Ranges;
               printf ("   discreteRanges: \n");
               processDiscreteRangeList(discreteRanges);
               break;
             }

       // A_Discriminant_Constraint
          case A_Discriminant_Constraint:
             {
               Discriminant_Association_List & discriminantAssociations = x.Discriminant_Associations;
               printf ("   discriminantAssociations: \n");
               processDiscriminantAssociationList(discriminantAssociations);
               break;
             }

          default:
             {
               printf ("Default reached in processConstraint(): constraintKind = %d constraintKind = %s \n",constraintKind,constraintKindName(constraintKind).c_str());
               ROSE_ASSERT(false);
             }
        }

#if 0
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processComponentDefinition ( Component_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
     Definition Component_Definition_View;
#endif

     Definition componentDefinitionView = x.Component_Definition_View;
     printf ("   componentDefinitionView = %d \n",componentDefinitionView);

#if 0
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processDiscreteSubtypeDefinition ( Discrete_Subtype_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     //~ ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processDiscreteRange ( Discrete_Range_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     // ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processUnknownDiscriminantPart ( Unknown_Discriminant_Part_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processKnownDiscriminantPart ( Known_Discriminant_Part_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processRecordDefinition ( Record_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
     Record_Component_List Record_Components;
     Record_Component_List Implicit_Components;
#endif

     Record_Component_List & recordComponents    = x.Record_Components;
     printf ("   recordComponents: \n");
     processRecordComponentList(recordComponents);

     Record_Component_List & implicitComponents = x.Implicit_Components;
     printf ("   implicitComponents: \n");
     processRecordComponentList(implicitComponents);

#if 0
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processNullRecordDefinition ( Null_Record_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processNullComponent ( Null_Component_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processVariantPart ( Variant_Part_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processVariant ( Variant_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processOthersChoice ( Others_Choice_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     // ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processAccessDefinition ( Access_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 0
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processPrivateTypeDefinition ( Private_Type_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processTaggedPrivateTypeDefinition ( Tagged_Private_Type_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processPrivateExtensionDefinition ( Private_Extension_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processTaskDefinition ( Task_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
     bool                  Has_Task;
     Declarative_Item_List Visible_Part_Items;
     Declarative_Item_List Private_Part_Items;
     bool                  Is_Private_Present;
#endif

     bool hasTask = x.Has_Task;
     printf ("   hasTask = %s \n",hasTask ? "true" : "false");

     Declarative_Item_List & visiblePartItems = x.Visible_Part_Items;
     printf ("   visiblePartItems: \n");
     processDeclarativeItemList(visiblePartItems);

     Declarative_Item_List & privatePartItems = x.Private_Part_Items;
     printf ("   privatePartItems: \n");
     processDeclarativeItemList(privatePartItems);

     bool isPrivatePresent = x.Is_Private_Present;
     printf ("   isPrivatePresent = %s \n",isPrivatePresent ? "true" : "false");

#if 0
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processProtectedDefinition ( Protected_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processFormalTypeDefinition ( Formal_Type_Definition_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processAspectSpecification ( Aspect_Specification_Struct & x, int element_id)
   {
#if 0
  // Documentation for x
#endif

#if 1
     printf ("ERROR: not implemented! \n");
     ROSE_ASSERT(false);
#endif
   }




#if 0

// OLD CODE!

void
  Ada_ROSE_Translation::processDefinition( Definition_Struct & definition, int element_id)
   {
#if 0
  // Documentation for Definition_Struct.
  enum Definition_Kinds Definition_Kind;

#error "DEAD CODE!"

  // These fields are only valid for the kinds above them:
  // A_Component_Definition
  // A_Private_Type_Definition
  // A_Tagged_Private_Type_Definition
  // A_Private_Extension_Definition
  // A_Subtype_Indication
  // An_Access_Definition
  enum Trait_Kinds      Trait_Kind;
  // A_Type_Definition
  enum Type_Kinds       Type_Kind;
  // A_Derived_Type_Definition
  // A_Derived_Record_Extension_Definition
  Subtype_Indication_ID Parent_Subtype_Indication;
  // A_Derived_Record_Extension_Definition
  Definition_ID         Record_Definition;
  // A_Derived_Type_Definition
  // A_Derived_Record_Extension_Definition
  Definition_ID_List    Implicit_Inherited_Declarations;
  Definition_ID_List    Implicit_Inherited_Subprograms;
  Definition_ID         Corresponding_Parent_Subtype;
  Definition_ID         Corresponding_Root_Type;
  Definition_ID         Corresponding_Type_Structure;
  // A_Constraint
  enum Constraint_Kinds Constraint_Kind;
  // A_Simple_Expression_Range
  Expression_ID         Lower_Bound;
  Expression_ID         Upper_Bound;
  // A_Subtype_Indication
  // A_Discrete_Subtype_Definition (See Discrete_Range_Kinds)
  // A_Discrete_Range (See Discrete_Range_Kinds)
  Expression_ID         Subtype_Mark;
  Constraint_ID         Subtype_Constraint;
  // A_Component_Definition
  Subtype_Indication_ID Component_Subtype_Indication;
  Definition_ID         Component_Definition_View;

  // A_Record_Definition
  // A_Variant
  Record_Component_List Record_Components;
  Record_Component_List Implicit_Components;

  Declarative_Item_ID_List Visible_Part_Items;
  Declarative_Item_ID_List Private_Part_Items;
  bool                     Is_Private_Present;
  // TODO: not done yet - abt 55 fields to go.  Introduce union and sub structs?

#error "DEAD CODE!"

  // Thse are the Definition_Kinds:
enum Definition_Kinds {
  Not_A_Definition,                 // An unexpected element

  A_Type_Definition,                // 3.2.1(4)    -> Type_Kinds

  A_Subtype_Indication,             // 3.2.2(3)
  A_Constraint,                     // 3.2.2(5)    -> Constraint_Kinds

  A_Component_Definition,           // 3.6(7)      -> Trait_Kinds

  A_Discrete_Subtype_Definition,    // 3.6(6)      -> Discrete_Range_Kinds
  A_Discrete_Range,                 // 3.6.1(3)    -> Discrete_Range_Kinds

  An_Unknown_Discriminant_Part,     // 3.7(3)
  A_Known_Discriminant_Part,        // 3.7(2)

  A_Record_Definition,              // 3.8(3)
  A_Null_Record_Definition,         // 3.8(3)

  A_Null_Component,                 // 3.8(4)
  A_Variant_Part,                   // 3.8.1(2)
  A_Variant,                        // 3.8.1(3)

  An_Others_Choice,                 // 3.8.1(5), 4.3.1(5), 4.3.3(5), 11.2(5)

  //  //|A2005 start
  An_Access_Definition,             // 3.10(6/2)   -> Access_Definition_Kinds
  //  //|A2005 end

  A_Private_Type_Definition,        // 7.3(2)      -> Trait_Kinds
  A_Tagged_Private_Type_Definition, // 7.3(2)      -> Trait_Kinds
  A_Private_Extension_Definition,   // 7.3(3)      -> Trait_Kinds

  A_Task_Definition,                // 9.1(4)
  A_Protected_Definition,           // 9.4(4)

  A_Formal_Type_Definition,         // 12.5(3)     -> Formal_Type_Kinds

  //  //|A2012 start
  An_Aspect_Specification           // 13.3.1
  //  //|A2012 end
};

#error "DEAD CODE!"

#endif

     Definition_Kinds definitionKind = definition.Definition_Kind;

#error "DEAD CODE!"

     printf ("   In processDefinition(): \n");
     printf ("      definitionKind (value) = %d \n",definitionKind);
     printf ("      definitionKind (name)  = %s \n",definitionKindName(definitionKind).c_str());

#error "DEAD CODE!"

     switch (definitionKind)
        {
       // A_Component_Definition
       // A_Private_Type_Definition
       // A_Tagged_Private_Type_Definition
       // A_Private_Extension_Definition
       // A_Subtype_Indication
       // An_Access_Definition
       // case A_Component_Definition:
          case A_Private_Type_Definition:
          case A_Tagged_Private_Type_Definition:
          case A_Private_Extension_Definition:
       // case A_Subtype_Indication:
          case An_Access_Definition:
             {
            // Trait_Kinds traitKind = definition.Trait_Kind;
            // printf ("     traitKind (value) = %d \n",traitKind);
            // printf ("     traitKind (name)  = %s \n",traitKindName(traitKind).c_str());
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#error "DEAD CODE!"

       // A_Type_Definition
          case A_Type_Definition:
             {
               Type_Kinds typeKind = definition.Type_Kind;
               printf ("     typeKind (value) = %d \n",typeKind);
            // printf ("     typeKind (name)  = %s \n",typeKindName(typeKind).c_str());

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#error "DEAD CODE!"

#if 0
       // A_Derived_Type_Definition
       // A_Derived_Record_Extension_Definition
          case A_Derived_Type_Definition:
       // case A_Derived_Record_Extension_Definition:
             {
               Subtype_Indication_ID parentSubtypeIndication = definition.Parent_Subtype_Indication;
               printf ("     parentSubtypeIndication = %d \n",parentSubtypeIndication);

            // A_Derived_Type_Definition
            // A_Derived_Record_Extension_Definition
               Definition_ID_List & implicitInheritedDeclarations = definition.Implicit_Inherited_Declarations;
               printf ("     implicitInheritedDeclarations: \n");
               processPathList(implicitInheritedDeclarations);
               Definition_ID_List & implicitInheritedSubprograms = definition.Implicit_Inherited_Subprograms;
               printf ("     implicitInheritedSubprograms: \n");
               processPathList(implicitInheritedSubprograms);
               Definition_ID correspondingParentSubtype = definition.Corresponding_Parent_Subtype;
               printf ("     correspondingParentSubtype = %d \n",correspondingParentSubtype);
               Definition_ID correspondingRootType = definition.Corresponding_Root_Type;
               printf ("     correspondingRootType = %d \n",correspondingRootType);
               Definition_ID correspondingTypeStructure = definition.Corresponding_Type_Structure;
               printf ("     correspondingTypeStructure = %d \n",correspondingTypeStructure);
               break;
             }
#endif

#error "DEAD CODE!"

#if 0
       // A_Derived_Type_Definition
       // A_Derived_Record_Extension_Definition
       // case A_Derived_Type_Definition:
          case A_Derived_Record_Extension_Definition:
             {
               Subtype_Indication_ID parentSubtypeIndication = definition.Parent_Subtype_Indication;
               printf ("     parentSubtypeIndication = %d \n",parentSubtypeIndication);

            // A_Derived_Record_Extension_Definition
               Definition_ID recordDefinition = definition.Record_Definition;
               printf ("     recordDefinition = %d \n",recordDefinition);

            // A_Derived_Type_Definition
            // A_Derived_Record_Extension_Definition
               Definition_ID_List & implicitInheritedDeclarations = definition.Implicit_Inherited_Declarations;
               printf ("     implicitInheritedDeclarations: \n");
               processDefinitionIdList(implicitInheritedDeclarations);
               Definition_ID_List & implicitInheritedSubprograms = definition.Implicit_Inherited_Subprograms;
               printf ("     implicitInheritedSubprograms: \n");
               processDefinitionIdList(implicitInheritedSubprograms);
               Definition_ID correspondingParentSubtype = definition.Corresponding_Parent_Subtype;
               printf ("     correspondingParentSubtype = %d \n",correspondingParentSubtype);
               Definition_ID correspondingRootType = definition.Corresponding_Root_Type;
               printf ("     correspondingRootType = %d \n",correspondingRootType);
               Definition_ID correspondingTypeStructure = definition.Corresponding_Type_Structure;
               printf ("     correspondingTypeStructure = %d \n",correspondingTypeStructure);
               break;
             }
#endif

#error "DEAD CODE!"

       // A_Constraint
          case A_Constraint:
             {
               Constraint_Kinds constraintKind = definition.Constraint_Kind;
               printf ("     constraintKind (value) = %d \n",constraintKind);
            // printf ("     constraintKind (name)  = %s \n",constraintKindName(constraintKind).c_str());

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#error "DEAD CODE!"

#if 0
       // A_Simple_Expression_Range
          case A_Simple_Expression_Range:
             {
               Expression_ID lowerBound = definition.Lower_Bound;
               printf ("     lowerBound = %d \n",lowerBound);
               Expression_ID upperBound = definition.Upper_Bound;
               printf ("     upperBound = %d \n",upperBound);
               break;
             }
#endif
       // A_Subtype_Indication
       // A_Discrete_Subtype_Definition (See Discrete_Range_Kinds)
       // A_Discrete_Range (See Discrete_Range_Kinds)
       // case A_Subtype_Indication:
          case A_Discrete_Subtype_Definition:
          case A_Discrete_Range:
             {
               Expression_ID subtypeMark = definition.Subtype_Mark;
               printf ("     subtypeMark = %d \n",subtypeMark);
               Constraint_ID subtypeConstraint = definition.Subtype_Constraint;
               printf ("     subtypeConstraint = %d \n",subtypeConstraint);

#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#error "DEAD CODE!"

          case A_Subtype_Indication:
             {
               Expression_ID subtypeMark = definition.Subtype_Mark;
               printf ("     subtypeMark = %d \n",subtypeMark);
               Constraint_ID subtypeConstraint = definition.Subtype_Constraint;
               printf ("     subtypeConstraint = %d \n",subtypeConstraint);

            // Trait_Kinds traitKind = definition.Trait_Kind;
            // printf ("     traitKind (value) = %d \n",traitKind);
            // printf ("     traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#error "DEAD CODE!"

       // A_Component_Definition
          case A_Component_Definition:
             {
            // Trait_Kinds traitKind = definition.Trait_Kind;
            // printf ("     traitKind (value) = %d \n",traitKind);
            // printf ("     traitKind (name)  = %s \n",traitKindName(traitKind).c_str());

               Subtype_Indication_ID componentSubtypeIndication = definition.Component_Subtype_Indication;
               printf ("     componentSubtypeIndication = %d \n",componentSubtypeIndication);
               Definition_ID componentDefinitionView            = definition.Component_Definition_View;
               printf ("     componentDefinitionView = %d \n",componentDefinitionView);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#error "DEAD CODE!"

       // A_Record_Definition
       // A_Variant
          case A_Record_Definition:
          case A_Variant:
             {
               Record_Component_List & recordComponents = definition.Record_Components;
               printf ("     recordComponents: \n");
               processRecordComponentList(recordComponents);
               Record_Component_List & implicitComponents = definition.Implicit_Components;
               printf ("     implicitComponents: \n");
               processRecordComponentList(implicitComponents);
               Declarative_Item_ID_List & visiblePartItems = definition.Visible_Part_Items;
               printf ("     visiblePartItems: \n");
               processDeclarativeItemIdList(visiblePartItems);
               Declarative_Item_ID_List & privatePartItems = definition.Private_Part_Items;
               printf ("     privatePartItems: \n");
               processDeclarativeItemIdList(privatePartItems);
               bool isPrivatePresent = definition.Is_Private_Present;
               printf ("     isPrivatePresent = %s \n",isPrivatePresent ? "true" : "false");

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#error "DEAD CODE!"

          case A_Task_Definition:
             {
            // Nothing to output here, so far!

               printf ("Need to build a task definition! \n");

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#error "DEAD CODE!"

          default:
             {
               printf ("Default reached in processDefinition(): definitionKind = %d definitionKind = %s \n",definitionKind,definitionKindName(definitionKind).c_str());
               ROSE_ASSERT(false);
             }
        }

#error "DEAD CODE!"

  // DQ (10/5/2017): This is general code to handled definitions, we likely new additional
  // untyped IR nodes to support the concept fo a SgUntypedDefinition, plus whatever
  // variations of untyped definition IR nodes that we require (for the cases above).
     SgUntypedScope* untypedScope = new SgUntypedScope();
     ROSE_ASSERT(untypedScope != NULL);

     SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
     ROSE_ASSERT(untypedStatementList != NULL);

  // Connect IR nodes.
     untypedScope->set_statement_list(untypedStatementList);

     printf ("In processDefinition(): element_id = %d \n",element_id);

#error "DEAD CODE!"

  // ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
     if (untypedNodeMap.find(element_id) == untypedNodeMap.end())
        {
          untypedNodeMap[element_id] = untypedScope;
        }
       else
        {
          printf ("ERROR: ignoring redundent processing of definition: definitionKind = %d definitionKind = %s \n",definitionKind,definitionKindName(definitionKind).c_str());
        }
     ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

     setAsisAttribute (untypedScope,element_id);
   }

#error "DEAD CODE!"

#endif


void
Ada_ROSE_Translation::processClause( Clause_Struct & clause)
   {
#if 0
  // Documentation for Clause_Struct.
     enum Clause_Kinds Clause_Kind;
  // These fields are only valid for the kinds above them:
  //   A_Use_Package_Clause
  //   A_Use_Type_Clause
  //   A_Use_All_Type_Clause
  //   A_With_Clause
     Name_List         Clause_Names;
  //   A_With_Clause
     enum Trait_Kinds  Trait_Kind;
#endif

     Clause_Kinds clauseKind = clause.Clause_Kind;

     printf ("   In processClause(): \n");
     printf ("      clauseKind (value) = %d \n",clauseKind);
     printf ("      clauseKind (name)  = %s \n",clauseKindName(clauseKind).c_str());

   }

void
Ada_ROSE_Translation::processAssociation( Association_Struct & association, int element_id)
   {
#if 0
  // Documentation for Association_Struct.
  // An_Array_Component_Association,        // 4.3.3
  Expression_List        Array_Component_Choices;
  // A_Record_Component_Association,        // 4.3.1
  Expression_List        Record_Component_Choices;
  // An_Array_Component_Association,        // 4.3.3
  // A_Record_Component_Association,        // 4.3.1
  Expression_ID          Component_Expression;
  // A_Pragma_Argument_Association,         // 2.8
  // A_Parameter_Association,               // 6.4
  // A_Generic_Association                  // 12.3
  Expression_ID          Formal_Parameter;
  Expression_ID          Actual_Parameter;
  // A_Discriminant_Association,            // 3.7.1
  Expression_List        Discriminant_Selector_Names;
  Expression_ID          Discriminant_Expression;
  // A_Discriminant_Association,            // 3.7.1
  // A_Record_Component_Association,        // 4.3.1
  // A_Parameter_Association,               // 6.4
  // A_Generic_Association                  // 12.3
  bool                   Is_Normalized;
  // A_Parameter_Association
  // A_Generic_Association
  // A_Record_Component_Association
  bool                   Is_Defaulted_Association;
#endif

     Association_Kinds associationKind = association.Association_Kind;

     printf ("   In processAssociation(): \n");
     printf ("      associationKind (value) = %d \n",associationKind);
     printf ("      associationKind (name)  = %s \n",associationKindName(associationKind).c_str());

     switch (associationKind)
        {

       // An_Array_Component_Association,        // 4.3.3
          case An_Array_Component_Association:
             {
               Expression_List & arrayComponentChoices = association.Array_Component_Choices;
               printf ("     arrayComponentChoices: \n");
               processExpressionList(arrayComponentChoices);

               Expression_ID componentExpression = association.Component_Expression;
               printf ("     componentExpression = %d \n",componentExpression);

#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Record_Component_Association,        // 4.3.1
          case A_Record_Component_Association:
             {
               Expression_List & recordComponentChoices = association.Record_Component_Choices;
               printf ("     recordComponentChoices: \n");
               processExpressionList(recordComponentChoices);

               Expression_ID componentExpression = association.Component_Expression;
               printf ("     componentExpression = %d \n",componentExpression);

               bool isNormalized = association.Is_Normalized;
               printf ("     isNormalized = %s \n",isNormalized ? "true" : "false");

               bool isDefaultedAssociation = association.Is_Defaulted_Association;
               printf ("     isDefaultedAssociation = %s \n",isDefaultedAssociation ? "true" : "false");
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // An_Array_Component_Association,        // 4.3.3
       // A_Record_Component_Association,        // 4.3.1
       // case An_Array_Component_Association:
       // case A_Record_Component_Association:
             {
               Expression_ID componentExpression = association.Component_Expression;
               printf ("     componentExpression = %d \n",componentExpression);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }
#endif
       // A_Pragma_Argument_Association,         // 2.8
       // A_Parameter_Association,               // 6.4
       // A_Generic_Association                  // 12.3
          case A_Pragma_Argument_Association:
       // case A_Parameter_Association:
       // case A_Generic_Association:
             {
               Expression_ID formalParameter = association.Formal_Parameter;
               printf ("     formalParameter = %d \n",formalParameter);
               Expression_ID actualParameter = association.Actual_Parameter;
               printf ("     actualParameter = %d \n",actualParameter);
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          case A_Parameter_Association:
          case A_Generic_Association:
             {
               Expression_ID formalParameter = association.Formal_Parameter;
               printf ("     formalParameter = %d \n",formalParameter);
               Expression_ID actualParameter = association.Actual_Parameter;
               printf ("     actualParameter = %d \n",actualParameter);

               bool isNormalized = association.Is_Normalized;
               printf ("     isNormalized = %s \n",isNormalized ? "true" : "false");

               bool isDefaultedAssociation = association.Is_Defaulted_Association;
               printf ("     isDefaultedAssociation = %s \n",isDefaultedAssociation ? "true" : "false");

               SgUntypedExpression* untypedExpression = NULL;
               if (associationKind == A_Parameter_Association)
                  {
                    untypedExpression = new SgUntypedExpression(e_parameterAssociation);
                  }
                 else
                  {
                    untypedExpression = new SgUntypedExpression(e_genericAssociation);
                  }

               ROSE_ASSERT(untypedExpression != NULL);

            // ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               if (untypedNodeMap.find(element_id) == untypedNodeMap.end())
                  {
                    processUntypedNode(untypedExpression,element_id);
                  }
                 else
                  {
                    printf ("ERROR: redundant use of element_id = %d \n",element_id);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Discriminant_Association,            // 3.7.1
          case A_Discriminant_Association:
             {
               Expression_List & discriminantSelectorNames = association.Discriminant_Selector_Names;
               printf ("     discriminantSelectorNames: \n");
               processExpressionList(discriminantSelectorNames);
               Expression_ID discriminantExpression = association.Discriminant_Expression;
               printf ("     discriminantExpression = %d \n",discriminantExpression);

               bool isNormalized = association.Is_Normalized;
               printf ("     isNormalized = %s \n",isNormalized ? "true" : "false");
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

#if 0
       // A_Discriminant_Association,            // 3.7.1
       // A_Record_Component_Association,        // 4.3.1
       // A_Parameter_Association,               // 6.4
       // A_Generic_Association                  // 12.3
       // case A_Discriminant_Association:
       // case A_Record_Component_Association:
       // case A_Parameter_Association:
       // case A_Generic_Association:
             {
               bool isNormalized = association.Is_Normalized;
               printf ("     isNormalized = %s \n",isNormalized ? "true" : "false");
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }
#endif
#if 0
       // A_Parameter_Association
       // A_Generic_Association
       // A_Record_Component_Association
       // case A_Parameter_Association:
       // case A_Generic_Association:
       // case A_Record_Component_Association:
             {
               bool isDefaultedAssociation = association.Is_Defaulted_Association;
               printf ("     isDefaultedAssociation = %s \n",isDefaultedAssociation ? "true" : "false");
#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }
#endif
          default:
             {
               printf ("Default reached in processAssociation(): associationKind = %d associationKind = %s \n",associationKind,associationKindName(associationKind).c_str());
               ROSE_ASSERT(false);
             }
        }

   }


void
Ada_ROSE_Translation::processExceptionHandler( Exception_Handler_Struct & exceptionHandler, int element_id)
   {
#if 0
  // Documentation for Exception_Handler_Struct.
     Declaration_ID Choice_Parameter_Specification;
     Element_List   Exception_Choices;
     Statement_List Handler_Statements;
#endif

     Declaration_ID declarationId = exceptionHandler.Choice_Parameter_Specification;

     printf ("   In processAssociation(): \n");
     printf ("      declarationId = %d \n",declarationId);

     Element_ID_List & exceptionChoices = exceptionHandler.Exception_Choices;
     printf ("     exceptionChoices: \n");
     processExpressionList(exceptionChoices);
     Statement_List & handlerStatements = exceptionHandler.Handler_Statements;
     printf ("     handlerStatements: \n");
     processExpressionList(handlerStatements);

     SgUntypedExceptionHandlerDeclaration* untypedExceptionHandlerDeclaration = new SgUntypedExceptionHandlerDeclaration();
     ROSE_ASSERT(untypedExceptionHandlerDeclaration != NULL);

     ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
     processUntypedNode(untypedExceptionHandlerDeclaration,element_id);
     ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());
   }


void
Ada_ROSE_Translation::processPath( Path_Struct & path, int element_id)
   {
#if 0
  // Documentation for Path_Struct.
  enum Path_Kinds Path_Kind;
  // These fields are only valid for the kinds above them:
  // An_If_Path,
  // An_Elsif_Path,
  Expression_ID  Condition_Expression;
  // (all)
  Statement_List Sequence_Of_Statements;
  // A_Case_Path,
  // A_Case_Expression_Path,
  Element_List   Case_Path_Alternative_Choices;
  // A_Select_Path,
  // An_Or_Path,
  Expression_ID  Guard;


enum Path_Kinds {
  Not_A_Path,
  //  An unexpected element

  //  Statement paths:

  An_If_Path,
  //  5.3:
  //  if condition then
  //    sequence_of_statements
  An_Elsif_Path,
  //  5.3:
  //  elsif condition then
  //    sequence_of_statements
  An_Else_Path,
  //  5.3, 9.7.1, 9.7.3:
  //  else sequence_of_statements
  A_Case_Path,
  //  5.4:
  //  when discrete_choice_list =>
  //    sequence_of_statements
  A_Select_Path,
  //  9.7.1:
  //     select [guard] select_alternative
  //  9.7.2, 9.7.3:
  //     select entry_call_alternative
  //  9.7.4:
  //     select triggering_alternative
  An_Or_Path,
  //  9.7.1:
  //     or [guard] select_alternative
  //  9.7.2:
  //     or delay_alternative
  A_Then_Abort_Path,
  //  9.7.4
  //     then abort sequence_of_statements

  //  //|A2012 start
  //  Expression paths:

  A_Case_Expression_Path,
  //  ??? (RM 2012)
  //  when expression => expression
  An_If_Expression_Path,
  //  ??? (RM 2012)
  //  if condition then expression
  An_Elsif_Expression_Path,
  //  ??? (RM 2012)
  //  elsif condition then expression
  An_Else_Expression_Path
  //  ??? (RM 2012)
  //  else expression
};
#endif

     Path_Kinds pathKind = path.Path_Kind;

     printf ("   In processPath(): \n");
     printf ("      pathKind (value) = %d \n",pathKind);
     printf ("      pathKind (name)  = %s \n",pathKindName(pathKind).c_str());

     Statement_List & sequenceOfStatements = path.Sequence_Of_Statements;
     printf ("      sequenceOfStatements: \n");
     processElementList(sequenceOfStatements);

     switch(pathKind)
        {
       // An_If_Path,
       // An_Elsif_Path,
          case An_If_Path:
          case An_Elsif_Path:
             {
#if 0
               Expression_ID conditionExpression = path.Condition_Expression;
               printf ("      conditionExpression = %d \n",conditionExpression);

               SgUntypedIfStatement* untypedIfStatement = new SgUntypedIfStatement();
               ROSE_ASSERT(untypedIfStatement != NULL);
#endif
               SgUntypedScope* untypedScope = new SgUntypedScope();
               ROSE_ASSERT(untypedScope != NULL);

            // This should be handled for the true case only.
            // untypedIfStatement->set_statement(untypedScope);

               SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
               ROSE_ASSERT(untypedStatementList != NULL);

            // Connect IR nodes.
               untypedScope->set_statement_list(untypedStatementList);

            // Note that we can't connect the statements in the function body, until a second pass over the untyped AST.

               printf ("element_id = %d \n",element_id);

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
            // untypedNodeMap[element_id] = untypedIfStatement;
               untypedNodeMap[element_id] = untypedScope;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

            // setAsisAttribute (untypedIfStatement,element_id);
               setAsisAttribute (untypedScope,element_id);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Case_Path,
       // A_Case_Expression_Path,
          case A_Case_Path:
          case A_Case_Expression_Path:
             {
               Element_ID_List & casePathAlternativeChoices = path.Case_Path_Alternative_Choices;
               printf ("      casePathAlternativeChoices: \n");
               processElementList(casePathAlternativeChoices);

#if 1
               printf ("Exiting as a test! \n");
               // ROSE_ASSERT(false);
#endif
               break;
             }

       // A_Select_Path,
       // An_Or_Path,
          case A_Select_Path:
          case An_Or_Path:
             {
               Expression_ID guard = path.Guard;
               printf ("      guard = %d \n",guard);

#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // 5.3, 9.7.1, 9.7.3: else sequence_of_statements
          case An_Else_Path:
             {
               printf ("case An_Else_Path in processPath(): \n");

            // Find the associated SgUntypedIfStatement and fill in the else case.
            // However, I think we have not seen the if statment yet, so we can't build it until then.

               SgUntypedScope* untypedScope = new SgUntypedScope();
               ROSE_ASSERT(untypedScope != NULL);

               SgUntypedStatementList* untypedStatementList = new SgUntypedStatementList();
               ROSE_ASSERT(untypedStatementList != NULL);

            // Connect IR nodes.
               untypedScope->set_statement_list(untypedStatementList);

            // Note that we can't connect the statements in the function body, until a second pass over the untyped AST.

               ROSE_ASSERT(untypedNodeMap.find(element_id) == untypedNodeMap.end());
               untypedNodeMap[element_id] = untypedScope;
               ROSE_ASSERT(untypedNodeMap.find(element_id) != untypedNodeMap.end());

               setAsisAttribute (untypedScope,element_id);

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // 9.7.4: then abort sequence_of_statements
          case A_Then_Abort_Path:

       // ??? (RM 2012): if condition then expression
          case An_If_Expression_Path:

       // ??? (RM 2012): elsif condition then expression
          case An_Elsif_Expression_Path:

       // ??? (RM 2012): else expression
          case An_Else_Expression_Path:
             {
               printf ("Simple case reached in processPath(): pathKind = %d pathKind = %s \n",pathKind,pathKindName(pathKind).c_str());

#if 1
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               printf ("Default reached in processPath(): pathKind = %d pathKind = %s \n",pathKind,pathKindName(pathKind).c_str());
               ROSE_ASSERT(false);
             }
        }

   }


void
Ada_ROSE_Translation::processDefiningNameList( Defining_Name_List & labelNames)
   {
#if 1
     printf ("In processDefiningNameList(): length = %d \n",labelNames.Length);
#endif

     processElementIdList(labelNames);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processPathList( Path_List & pathList)
   {
#if 1
     printf ("In processPathList(): length = %d \n",pathList.Length);
#endif

     processElementIdList(pathList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processAssociationList( Association_List & associationList)
   {
#if 1
     printf ("In processAssociationList(): length = %d \n",associationList.Length);
#endif

     processElementIdList(associationList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processStatementList( Statement_List & statementList)
   {
#if 1
     printf ("In processStatementList(): length = %d \n",statementList.Length);
#endif

     processElementIdList(statementList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processParameterSpecificationList( Parameter_Specification_List & parameterSpecificationList)
   {
#if 1
     printf ("In processParameterSpecificationListList(): length = %d \n",parameterSpecificationList.Length);
#endif

     processElementIdList(parameterSpecificationList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processElementList( Element_ID_List & elementList)
   {
#if 1
     printf ("In processElementList(): length = %d \n",elementList.Length);
#endif

     processElementIdList(elementList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }



void
Ada_ROSE_Translation::processRepresentationClauseList( Representation_Clause_List & representationClauseList)
   {
#if 1
     printf ("In processRepresentationClauseList(): length = %d \n",representationClauseList.Length);
#endif

     processElementIdList(representationClauseList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processDeclarativeItemList( Declarative_Item_List & declarativeItemList)
   {
#if 1
     printf ("In processDeclarativeItemList(): length = %d \n",declarativeItemList.Length);
#endif

     processElementIdList(declarativeItemList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processExceptionHandlerList( Exception_Handler_List & exceptionHandlerList)
   {
#if 1
     printf ("In processExceptionHandlerList(): length = %d \n",exceptionHandlerList.Length);
#endif

     processElementIdList(exceptionHandlerList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processExpressionList( Expression_List & expressionList)
   {
#if 1
     printf ("In processExpressionList(): length = %d \n",expressionList.Length);
#endif

     processElementIdList(expressionList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
Ada_ROSE_Translation::processDiscreteRangeList( Discrete_Range_List & discreteRangesList)
   {
#if 1
     printf ("In processDiscreteRangeList(): length = %d \n",discreteRangesList.Length);
#endif

     processElementIdList(discreteRangesList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processDiscriminantAssociationList ( Discriminant_Association_List & discriminantAssociationList)
   {
#if 1
     printf ("In processDiscriminantAssociationList(): length = %d \n",discriminantAssociationList.Length);
#endif

     processElementIdList(discriminantAssociationList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processDeclarationList           ( Declaration_List & declarationList)
   {
#if 1
     printf ("In processDeclarationList(): length = %d \n",declarationList.Length);
#endif

     processElementIdList(declarationList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }



void
Ada_ROSE_Translation::processUnitList           ( Unit_List & unitList)
   {
#if 1
     printf ("In processUnitList(): length = %d \n",unitList.Length);
#endif

     processUnitIdList(unitList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processContentClauseList ( Context_Clause_List & contentClauseList)
   {
#if 1
     printf ("In processContentClauseList(): length = %d \n",contentClauseList.Length);
#endif

     processElementIdList(contentClauseList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processPragmaElementList ( Pragma_Element_ID_List & pragmaElementList)
   {
#if 1
     printf ("In processPragmaElementList(): length = %d \n",pragmaElementList.Length);
#endif

     processElementIdList(pragmaElementList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Ada_ROSE_Translation::processRecordComponentList       ( Record_Component_List & recordComponentList)
   {
#if 1
     printf ("In processRecordComponenList(): length = %d \n",recordComponentList.Length);
#endif

     processElementIdList(recordComponentList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

#if 0
void
Ada_ROSE_Translation::processDeclarativeItemIdList     ( Declarative_Item_ID_List & declarativeItemIdList)
   {
#if 1
     printf ("In processDeclarativeItemIdList(): length = %d \n",declarativeItemIdList.Length);
#endif

     processElementIdList(declarativeItemIdList);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }
#endif

void
Ada_ROSE_Translation::processElementIdList( Element_ID_Array_Struct & elementIdList)
   {
  // Iterate over the list.
     for (int i=0; i < elementIdList.Length; i++)
        {
          int id = elementIdList.IDs[i];
          printf ("   --- id = %d \n",id);
        }
   }

void
Ada_ROSE_Translation::processUnitIdList( Unit_ID_Array_Struct & unitIdList)
   {
  // Iterate over the list.
     for (int i=0; i < unitIdList.Length; i++)
        {
          int id = unitIdList.IDs[i];
          printf ("   --- id = %d \n",id);
        }
   }

string
Ada_ROSE_Translation::elementKindName (Element_Kinds x)
   {
#if 0
  // Enum values
     Not_An_Element,            // Nil_Element
     A_Pragma,                  // Asis.Elements
     A_Defining_Name,           // Asis.Declarations
     A_Declaration,             // Asis.Declarations
     A_Definition,              // Asis.Definitions
     An_Expression,             // Asis.Expressions
     An_Association,            // Asis.Expressions
     A_Statement,               // Asis.Statements
     A_Path,                    // Asis.Statements
     A_Clause,                  // Asis.Clauses
     An_Exception_Handler       // Asis.Statements
#endif

     string s;

     switch (x)
        {
          case Not_An_Element:       s = "Not_An_Element";       break;
          case A_Pragma:             s = "A_Pragma";             break;
          case A_Defining_Name:      s = "A_Defining_Name";      break;
          case A_Declaration:        s = "A_Declaration";        break;
          case A_Definition:         s = "A_Definition";         break;
          case An_Expression:        s = "An_Expression";        break;
          case An_Association:       s = "An_Association";       break;
          case A_Statement:          s = "A_Statement";          break;
          case A_Path:               s = "A_Path";               break;
          case A_Clause:             s = "A_Clause";             break;
          case An_Exception_Handler: s = "An_Exception_Handler"; break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::elementKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::enclosingKindName (Enclosing_Kinds x)
   {
#if 0
  // Enum values
     Not_Enclosing,
     Enclosing_Element,
     Enclosing_Unit
#endif

     string s;

     switch (x)
        {
          case Not_Enclosing:     s = "Not_Enclosing";     break;
          case Enclosing_Element: s = "Enclosing_Element"; break;
          case Enclosing_Unit:    s = "Enclosing_Unit";    break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::enclosingKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }



string
Ada_ROSE_Translation::statementKindName (Statement_Kinds x)
   {
#if 0
  // Enum values
     Not_A_Statement,                     // An unexpected element

     A_Null_Statement,                    // 5.1
     An_Assignment_Statement,             // 5.2
     An_If_Statement,                     // 5.3
     A_Case_Statement,                    // 5.4

     A_Loop_Statement,                    // 5.5
     A_While_Loop_Statement,              // 5.5
     A_For_Loop_Statement,                // 5.5
     A_Block_Statement,                   // 5.6
     An_Exit_Statement,                   // 5.7
     A_Goto_Statement,                    // 5.8

     A_Procedure_Call_Statement,          // 6.4
     A_Return_Statement,                  // 6.5
  //  //|A2005 start
     An_Extended_Return_Statement,        // 6.5
  //  //|A2005 end

     An_Accept_Statement,                 // 9.5.2
     An_Entry_Call_Statement,             // 9.5.3

     A_Requeue_Statement,                 // 9.5.4
     A_Requeue_Statement_With_Abort,      // 9.5.4

     A_Delay_Until_Statement,             // 9.6
     A_Delay_Relative_Statement,          // 9.6

     A_Terminate_Alternative_Statement,   // 9.7.1
     A_Selective_Accept_Statement,        // 9.7.1
     A_Timed_Entry_Call_Statement,        // 9.7.2
     A_Conditional_Entry_Call_Statement,  // 9.7.3
     An_Asynchronous_Select_Statement,    // 9.7.4

     An_Abort_Statement,                  // 9.8
     A_Raise_Statement,                   // 11.3
     A_Code_Statement                     // 13.8
#endif

     string s;

     switch (x)
        {
          case Not_A_Statement:                    s = "Not_A_Statement";                    break;
          case A_Null_Statement:                   s = "A_Null_Statement";                   break;
          case An_Assignment_Statement:            s = "An_Assignment_Statement";            break;
          case An_If_Statement:                    s = "An_If_Statement";                    break;
          case A_Case_Statement:                   s = "A_Case_Statement";                   break;
          case A_Loop_Statement:                   s = "A_Loop_Statement";                   break;
          case A_While_Loop_Statement:             s = "A_While_Loop_Statement";             break;
          case A_For_Loop_Statement:               s = "A_For_Loop_Statement";               break;
          case A_Block_Statement:                  s = "A_Block_Statement";                  break;
          case An_Exit_Statement:                  s = "An_Exit_Statement";                  break;
          case A_Goto_Statement:                   s = "A_Goto_Statement";                   break;
          case A_Procedure_Call_Statement:         s = "A_Procedure_Call_Statement";         break;
          case A_Return_Statement:                 s = "A_Return_Statement";                 break;
          case An_Extended_Return_Statement:       s = "An_Extended_Return_Statement";       break;
          case An_Accept_Statement:                s = "An_Accept_Statement";                break;
          case An_Entry_Call_Statement:            s = "An_Entry_Call_Statement";            break;
          case A_Requeue_Statement:                s = "A_Requeue_Statement";                break;
          case A_Requeue_Statement_With_Abort:     s = "A_Requeue_Statement_With_Abort";     break;
          case A_Delay_Until_Statement:            s = "A_Delay_Until_Statement";            break;
          case A_Delay_Relative_Statement:         s = "A_Delay_Relative_Statement";         break;
          case A_Terminate_Alternative_Statement:  s = "A_Terminate_Alternative_Statement";  break;
          case A_Selective_Accept_Statement:       s = "A_Selective_Accept_Statement";       break;
          case A_Timed_Entry_Call_Statement:       s = "A_Timed_Entry_Call_Statement";       break;
          case A_Conditional_Entry_Call_Statement: s = "A_Conditional_Entry_Call_Statement"; break;
          case An_Asynchronous_Select_Statement:   s = "An_Asynchronous_Select_Statement";   break;
          case An_Abort_Statement:                 s = "An_Abort_Statement";                 break;
          case A_Raise_Statement:                  s = "A_Raise_Statement";                  break;
          case A_Code_Statement:                   s = "A_Code_Statement";                   break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::statementKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::definingNameKindName (Defining_Name_Kinds x)
   {
#if 0
  // Enum values
     Not_A_Defining_Name,                       // An unexpected element
     A_Defining_Identifier,                     // 3.1(4)
     A_Defining_Character_Literal,              // 3.5.1(4)
     A_Defining_Enumeration_Literal,            // 3.5.1(3)
     A_Defining_Operator_Symbol,                // 6.1(9)
     A_Defining_Expanded_Name                   // 6.1(7)
#endif

     string s;

     switch (x)
        {
          case Not_A_Defining_Name:            s = "Not_A_Defining_Name";            break;
          case A_Defining_Identifier:          s = "A_Defining_Identifier";          break;
          case A_Defining_Character_Literal:   s = "A_Defining_Character_Literal";   break;
          case A_Defining_Enumeration_Literal: s = "A_Defining_Enumeration_Literal"; break;
          case A_Defining_Operator_Symbol:     s = "A_Defining_Operator_Symbol";     break;
          case A_Defining_Expanded_Name:       s = "A_Defining_Expanded_Name";       break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::definingNameKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::declarationKindName (Declaration_Kinds x)
   {
#if 0
  // Enum values
  Not_A_Declaration,                       // An unexpected element
  An_Ordinary_Type_Declaration,            // 3.2.1(3)
  //  a full_type_declaration of the form:
  //  type defining_identifier [known_discriminant_part] is
  //     type_definition;

  A_Task_Type_Declaration,                  // 9.1(2)
  A_Protected_Type_Declaration,             // 9.4(2)
  An_Incomplete_Type_Declaration,           // 3.2.1(2),3.10(2)
  //  //|A2005 start
  A_Tagged_Incomplete_Type_Declaration,     //  3.10.1(2)
  //  //|A2005 end
  A_Private_Type_Declaration,               // 3.2.1(2),7.3(2) -> Trait_Kinds
  A_Private_Extension_Declaration,          // 3.2.1(2),7.3(3) -> Trait_Kinds

  A_Subtype_Declaration,                    // 3.2.2(2)

  A_Variable_Declaration,                   // 3.3.1(2) -> Trait_Kinds
  A_Constant_Declaration,                   // 3.3.1(4) -> Trait_Kinds
  A_Deferred_Constant_Declaration,          // 3.3.1(6),7.4(2) -> Trait_Kinds
  A_Single_Task_Declaration,                // 3.3.1(2),9.1(3)
  A_Single_Protected_Declaration,           // 3.3.1(2),9.4(2)

  An_Integer_Number_Declaration,            // 3.3.2(2)
  A_Real_Number_Declaration,                // 3.5.6(2)

  An_Enumeration_Literal_Specification,     // 3.5.1(3)

  A_Discriminant_Specification,             // 3.7(5)   -> Trait_Kinds
  A_Component_Declaration,                  // 3.8(6)

  A_Loop_Parameter_Specification,           // 5.5(4)   -> Trait_Kinds
  //  //|A2012 start
  A_Generalized_Iterator_Specification,     // 5.5.2    -> Trait_Kinds
  An_Element_Iterator_Specification,        // 5.5.2    -> Trait_Kinds
  //  //|A2012 end

  A_Procedure_Declaration,                  // 6.1(4)   -> Trait_Kinds
  A_Function_Declaration,                   // 6.1(4)   -> Trait_Kinds

  A_Parameter_Specification,                // 6.1(15)  -> Trait_Kinds
  //                                                  -> Mode_Kinds
  A_Procedure_Body_Declaration,             // 6.3(2)
  A_Function_Body_Declaration,              // 6.3(2)

  //  //|A2005 start
  A_Return_Variable_Specification,          // 6.5
  A_Return_Constant_Specification,          // 6.5
  A_Null_Procedure_Declaration,             // 6.7
  //  //|A2005 end

  //  //|A2012 start
  An_Expression_Function_Declaration,       // 6.8
  //  //|A2012 end

  A_Package_Declaration,                    // 7.1(2)
  A_Package_Body_Declaration,               // 7.2(2)

  An_Object_Renaming_Declaration,           // 8.5.1(2)
  An_Exception_Renaming_Declaration,        // 8.5.2(2)
  A_Package_Renaming_Declaration,           // 8.5.3(2)
  A_Procedure_Renaming_Declaration,         // 8.5.4(2)
  A_Function_Renaming_Declaration,          // 8.5.4(2)
  A_Generic_Package_Renaming_Declaration,   // 8.5.5(2)
  A_Generic_Procedure_Renaming_Declaration, // 8.5.5(2)
  A_Generic_Function_Renaming_Declaration,  // 8.5.5(2)

  A_Task_Body_Declaration,                  // 9.1(6)
  A_Protected_Body_Declaration,             // 9.4(7)
  An_Entry_Declaration,                     // 9.5.2(2)
  An_Entry_Body_Declaration,                // 9.5.2(5)
  An_Entry_Index_Specification,             // 9.5.2(2)

  A_Procedure_Body_Stub,                    // 10.1.3(3)
  A_Function_Body_Stub,                     // 10.1.3(3)
  A_Package_Body_Stub,                      // 10.1.3(4)
  A_Task_Body_Stub,                         // 10.1.3(5)
  A_Protected_Body_Stub,                    // 10.1.3(6)

  An_Exception_Declaration,                 // 11.1(2)
  A_Choice_Parameter_Specification,         // 11.2(4)

  A_Generic_Procedure_Declaration,          // 12.1(2)
  A_Generic_Function_Declaration,           // 12.1(2)
  A_Generic_Package_Declaration,            // 12.1(2)

  A_Package_Instantiation,                  // 12.3(2)
  A_Procedure_Instantiation,                // 12.3(2)
  A_Function_Instantiation,                 // 12.3(2)

  A_Formal_Object_Declaration,              // 12.4(2)  -> Mode_Kinds
  A_Formal_Type_Declaration,                // 12.5(2)
  //  //|A2012 start
  A_Formal_Incomplete_Type_Declaration,
  //  //|A2012 end
  A_Formal_Procedure_Declaration,           // 12.6(2)
  //                                           -> Subprogram_Default_Kinds
  A_Formal_Function_Declaration,            // 12.6(2)
  //                                           -> Subprogram_Default_Kinds
  A_Formal_Package_Declaration,             // 12.7(2)
  A_Formal_Package_Declaration_With_Box     // 12.7(3)
#endif



     string s;

     switch (x)
        {
          case Not_A_Declaration:                        s = "Not_A_Declaration";                         break;
          case An_Ordinary_Type_Declaration:             s = "An_Ordinary_Type_Declaration";              break;
          case A_Task_Type_Declaration:                  s = "A_Task_Type_Declaration";                   break;
          case A_Protected_Type_Declaration:             s = "A_Protected_Type_Declaration";              break;
          case An_Incomplete_Type_Declaration:           s = "An_Incomplete_Type_Declaration";            break;
          case A_Tagged_Incomplete_Type_Declaration:     s = "A_Tagged_Incomplete_Type_Declaration";      break;
          case A_Private_Type_Declaration:               s = "A_Private_Type_Declaration";                break;
          case A_Private_Extension_Declaration:          s = "A_Private_Extension_Declaration";           break;
          case A_Subtype_Declaration:                    s = "A_Subtype_Declaration";                     break;
          case A_Variable_Declaration:                   s = "A_Variable_Declaration";                    break;
          case A_Constant_Declaration:                   s = "A_Constant_Declaration";                    break;
          case A_Deferred_Constant_Declaration:          s = "A_Deferred_Constant_Declaration";           break;
          case A_Single_Task_Declaration:                s = "A_Single_Task_Declaration";                 break;
          case A_Single_Protected_Declaration:           s = "A_Single_Protected_Declaration";            break;
          case An_Integer_Number_Declaration:            s = "An_Integer_Number_Declaration";             break;
          case A_Real_Number_Declaration:                s = "A_Real_Number_Declaration";                 break;
          case An_Enumeration_Literal_Specification:     s = "An_Enumeration_Literal_Specification";      break;
          case A_Discriminant_Specification:             s = "A_Discriminant_Specification";              break;
          case A_Component_Declaration:                  s = "A_Component_Declaration";                   break;
          case A_Loop_Parameter_Specification:           s = "A_Loop_Parameter_Specification";            break;
          case A_Generalized_Iterator_Specification:     s = "A_Generalized_Iterator_Specification";      break;
          case An_Element_Iterator_Specification:        s = "An_Element_Iterator_Specification";         break;
          case A_Procedure_Declaration:                  s = "A_Procedure_Declaration";                   break;
          case A_Function_Declaration:                   s = "A_Function_Declaration";                    break;
          case A_Parameter_Specification:                s = "A_Parameter_Specification";                 break;
          case A_Procedure_Body_Declaration:             s = "A_Procedure_Body_Declaration";              break;
          case A_Function_Body_Declaration:              s = "A_Function_Body_Declaration";               break;
          case A_Return_Variable_Specification:          s = "A_Return_Variable_Specification";           break;
          case A_Return_Constant_Specification:          s = "A_Return_Constant_Specification";           break;
          case A_Null_Procedure_Declaration:             s = "A_Null_Procedure_Declaration";              break;
          case An_Expression_Function_Declaration:       s = "An_Expression_Function_Declaration";        break;
          case A_Package_Declaration:                    s = "A_Package_Declaration";                     break;
          case A_Package_Body_Declaration:               s = "A_Package_Body_Declaration";                break;
          case An_Object_Renaming_Declaration:           s = "An_Object_Renaming_Declaration";            break;
          case An_Exception_Renaming_Declaration:        s = "An_Exception_Renaming_Declaration";         break;
          case A_Package_Renaming_Declaration:           s = "A_Package_Renaming_Declaration";            break;
          case A_Procedure_Renaming_Declaration:         s = "A_Procedure_Renaming_Declaration";          break;
          case A_Function_Renaming_Declaration:          s = "A_Function_Renaming_Declaration";           break;
          case A_Generic_Package_Renaming_Declaration:   s = "A_Generic_Package_Renaming_Declaration";    break;
          case A_Generic_Procedure_Renaming_Declaration: s = "A_Generic_Procedure_Renaming_Declaration";  break;
          case A_Generic_Function_Renaming_Declaration:  s = "A_Generic_Function_Renaming_Declaration";   break;
          case A_Task_Body_Declaration:                  s = "A_Task_Body_Declaration";                   break;
          case A_Protected_Body_Declaration:             s = "A_Protected_Body_Declaration";              break;
          case An_Entry_Declaration:                     s = "An_Entry_Declaration";                      break;
          case An_Entry_Body_Declaration:                s = "An_Entry_Body_Declaration";                 break;
          case An_Entry_Index_Specification:             s = "An_Entry_Index_Specification";              break;
          case A_Procedure_Body_Stub:                    s = "A_Procedure_Body_Stub";                     break;
          case A_Function_Body_Stub:                     s = "A_Function_Body_Stub";                      break;
          case A_Package_Body_Stub:                      s = "A_Package_Body_Stub";                       break;
          case A_Task_Body_Stub:                         s = "A_Task_Body_Stub";                          break;
          case A_Protected_Body_Stub:                    s = "A_Protected_Body_Stub";                     break;
          case An_Exception_Declaration:                 s = "An_Exception_Declaration";                  break;
          case A_Choice_Parameter_Specification:         s = "A_Choice_Parameter_Specification";          break;
          case A_Generic_Procedure_Declaration:          s = "A_Generic_Procedure_Declaration";           break;
          case A_Generic_Function_Declaration:           s = "A_Generic_Function_Declaration";            break;
          case A_Generic_Package_Declaration:            s = "A_Generic_Package_Declaration";             break;
          case A_Package_Instantiation:                  s = "A_Package_Instantiation";                   break;
          case A_Procedure_Instantiation:                s = "A_Procedure_Instantiation";                 break;
          case A_Function_Instantiation:                 s = "A_Function_Instantiation";                  break;
          case A_Formal_Object_Declaration:              s = "A_Formal_Object_Declaration";               break;
          case A_Formal_Type_Declaration:                s = "A_Formal_Type_Declaration";                 break;
          case A_Formal_Incomplete_Type_Declaration:     s = "A_Formal_Incomplete_Type_Declaration";      break;
          case A_Formal_Procedure_Declaration:           s = "A_Formal_Procedure_Declaration";            break;
          case A_Formal_Function_Declaration:            s = "A_Formal_Function_Declaration";             break;
          case A_Formal_Package_Declaration:             s = "A_Formal_Package_Declaration";              break;
          case A_Formal_Package_Declaration_With_Box:    s = "A_Formal_Package_Declaration_With_Box";     break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::declarationKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::expressionKindName (Expression_Kinds x)
   {
#if 0
  // Enum values
  Not_An_Expression,                         // An unexpected element

  A_Box_Expression,                          //  Ada 2005
  //  4.3.1(4), 4.3.3(3,6)

  An_Integer_Literal,                        // 2.4
  A_Real_Literal,                            // 2.4.1
  A_String_Literal,                          // 2.6

  An_Identifier,                             // 4.1
  An_Operator_Symbol,                        // 4.1
  A_Character_Literal,                       // 4.1
  An_Enumeration_Literal,                    // 4.1
  An_Explicit_Dereference,                   // 4.1
  A_Function_Call,                           // 4.1

  An_Indexed_Component,                      // 4.1.1
  A_Slice,                                   // 4.1.2
  A_Selected_Component,                      // 4.1.3
  An_Attribute_Reference,                    // 4.1.4  -> Attribute_Kinds

  A_Record_Aggregate,                        // 4.3
  An_Extension_Aggregate,                    // 4.3
  A_Positional_Array_Aggregate,              // 4.3
  A_Named_Array_Aggregate,                   // 4.3

  An_And_Then_Short_Circuit,                 // 4.4
  An_Or_Else_Short_Circuit,                  // 4.4

  An_In_Membership_Test,                     // 4.4  Ada 2012
  A_Not_In_Membership_Test,                  // 4.4  Ada 2012

  A_Null_Literal,                            // 4.4
  A_Parenthesized_Expression,                // 4.4
  A_Raise_Expression,                        // 4.4 Ada 2012 (AI12-0022-1)

  A_Type_Conversion,                         // 4.6
  A_Qualified_Expression,                    // 4.7

  An_Allocation_From_Subtype,                // 4.8
  An_Allocation_From_Qualified_Expression,   // 4.8
  A_Case_Expression,                         // Ada 2012
  An_If_Expression,                          // Ada 2012
  A_For_All_Quantified_Expression,           // Ada 2012
  A_For_Some_Quantified_Expression           // Ada 2012
#endif

     string s;

     switch (x)
        {
          case Not_An_Expression:                       s = "Not_An_Expression";                        break;
          case A_Box_Expression:                        s = "A_Box_Expression";                         break;
          case An_Integer_Literal:                      s = "An_Integer_Literal";                       break;
          case A_Real_Literal:                          s = "A_Real_Literal";                           break;
          case A_String_Literal:                        s = "A_String_Literal";                         break;
          case An_Identifier:                           s = "An_Identifier";                            break;
          case An_Operator_Symbol:                      s = "An_Operator_Symbol";                       break;
          case A_Character_Literal:                     s = "A_Character_Literal";                      break;
          case An_Enumeration_Literal:                  s = "An_Enumeration_Literal";                   break;
          case An_Explicit_Dereference:                 s = "An_Explicit_Dereference";                  break;
          case A_Function_Call:                         s = "A_Function_Call";                          break;
          case An_Indexed_Component:                    s = "An_Indexed_Component";                     break;
          case A_Slice:                                 s = "A_Slice";                                  break;
          case A_Selected_Component:                    s = "A_Selected_Component";                     break;
          case An_Attribute_Reference:                  s = "An_Attribute_Reference";                   break;
          case A_Record_Aggregate:                      s = "A_Record_Aggregate";                       break;
          case An_Extension_Aggregate:                  s = "An_Extension_Aggregate";                   break;
          case A_Positional_Array_Aggregate:            s = "A_Positional_Array_Aggregate";             break;
          case A_Named_Array_Aggregate:                 s = "A_Named_Array_Aggregate";                  break;
          case An_And_Then_Short_Circuit:               s = "An_And_Then_Short_Circuit";                break;
          case An_Or_Else_Short_Circuit:                s = "An_Or_Else_Short_Circuit";                 break;
          case An_In_Membership_Test:                   s = "An_In_Membership_Test";                    break;
          case A_Not_In_Membership_Test:                s = "A_Not_In_Membership_Test";                 break;
          case A_Null_Literal:                          s = "A_Null_Literal";                           break;
          case A_Parenthesized_Expression:              s = "A_Parenthesized_Expression";               break;
          case A_Raise_Expression:                      s = "A_Raise_Expression";                       break;
          case A_Type_Conversion:                       s = "A_Type_Conversion";                        break;
          case A_Qualified_Expression:                  s = "A_Qualified_Expression";                   break;
          case An_Allocation_From_Subtype:              s = "An_Allocation_From_Subtype";               break;
          case An_Allocation_From_Qualified_Expression: s = "An_Allocation_From_Qualified_Expression";  break;
          case A_Case_Expression:                       s = "A_Case_Expression";                        break;
          case An_If_Expression:                        s = "An_If_Expression";                         break;
          case A_For_All_Quantified_Expression:         s = "A_For_All_Quantified_Expression";          break;
          case A_For_Some_Quantified_Expression:        s = "A_For_Some_Quantified_Expression";         break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::expressionKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::definitionKindName (Definition_Kinds x)
   {
#if 0
  // Enum values
     Not_A_Definition,                 // An unexpected element

     A_Type_Definition,                // 3.2.1(4)    -> Type_Kinds

     A_Subtype_Indication,             // 3.2.2(3)
     A_Constraint,                     // 3.2.2(5)    -> Constraint_Kinds

     A_Component_Definition,           // 3.6(7)      -> Trait_Kinds

     A_Discrete_Subtype_Definition,    // 3.6(6)      -> Discrete_Range_Kinds
     A_Discrete_Range,                 // 3.6.1(3)    -> Discrete_Range_Kinds

     An_Unknown_Discriminant_Part,     // 3.7(3)
     A_Known_Discriminant_Part,        // 3.7(2)

     A_Record_Definition,              // 3.8(3)
     A_Null_Record_Definition,         // 3.8(3)

     A_Null_Component,                 // 3.8(4)
     A_Variant_Part,                   // 3.8.1(2)
     A_Variant,                        // 3.8.1(3)

     An_Others_Choice,                 // 3.8.1(5), 4.3.1(5), 4.3.3(5), 11.2(5)

  //  //|A2005 start
     An_Access_Definition,             // 3.10(6/2)   -> Access_Definition_Kinds
  //  //|A2005 end

     A_Private_Type_Definition,        // 7.3(2)      -> Trait_Kinds
     A_Tagged_Private_Type_Definition, // 7.3(2)      -> Trait_Kinds
     A_Private_Extension_Definition,   // 7.3(3)      -> Trait_Kinds

     A_Task_Definition,                // 9.1(4)
     A_Protected_Definition,           // 9.4(4)

     A_Formal_Type_Definition,         // 12.5(3)     -> Formal_Type_Kinds

  //  //|A2012 start
     An_Aspect_Specification           // 13.3.1
#endif

     string s;

     switch (x)
        {
          case Not_A_Definition:                 s = "Not_A_Definition";                 break;
          case A_Type_Definition:                s = "A_Type_Definition";                break;
          case A_Subtype_Indication:             s = "A_Subtype_Indication";             break;
          case A_Constraint:                     s = "A_Constraint";                     break;
          case A_Component_Definition:           s = "A_Component_Definition";           break;
          case A_Discrete_Subtype_Definition:    s = "A_Discrete_Subtype_Definition";    break;
          case A_Discrete_Range:                 s = "A_Discrete_Range";                 break;
          case An_Unknown_Discriminant_Part:     s = "An_Unknown_Discriminant_Part";     break;
          case A_Known_Discriminant_Part:        s = "A_Known_Discriminant_Part";        break;
          case A_Record_Definition:              s = "A_Record_Definition";              break;
          case A_Null_Record_Definition:         s = "A_Null_Record_Definition";         break;
          case A_Null_Component:                 s = "A_Null_Component";                 break;
          case A_Variant_Part:                   s = "A_Variant_Part";                   break;
          case A_Variant:                        s = "A_Variant";                        break;
          case An_Others_Choice:                 s = "An_Others_Choice";                 break;
          case An_Access_Definition:             s = "An_Access_Definition";             break;
          case A_Private_Type_Definition:        s = "A_Private_Type_Definition";        break;
          case A_Tagged_Private_Type_Definition: s = "A_Tagged_Private_Type_Definition"; break;
          case A_Private_Extension_Definition:   s = "A_Private_Extension_Definition";   break;
          case A_Task_Definition:                s = "A_Task_Definition";                break;
          case A_Protected_Definition:           s = "A_Protected_Definition";           break;
          case A_Formal_Type_Definition:         s = "A_Formal_Type_Definition";         break;
          case An_Aspect_Specification:          s = "An_Aspect_Specification";          break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::definitionKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::clauseKindName (Clause_Kinds x)
   {
#if 0
  // Enum values
     Not_A_Clause,                 // An unexpected element

     A_Use_Package_Clause,         // 8.4
     A_Use_Type_Clause,            // 8.4
     A_Use_All_Type_Clause,        // 8.4, Ada 2012

     A_With_Clause,                // 10.1.2

     A_Representation_Clause,      // 13.1     -> Representation_Clause_Kinds
     A_Component_Clause            // 13.5.1
#endif

     string s;

     switch (x)
        {
          case Not_A_Clause:            s = "Not_A_Clause";            break;
          case A_Use_Package_Clause:    s = "A_Use_Package_Clause";    break;
          case A_Use_Type_Clause:       s = "A_Use_Type_Clause";       break;
          case A_With_Clause:           s = "A_With_Clause";           break;
          case A_Representation_Clause: s = "A_Representation_Clause"; break;
          case A_Component_Clause:      s = "A_Component_Clause";      break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::clauseKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }



string
Ada_ROSE_Translation::associationKindName (Association_Kinds x)
   {
#if 0
  // Enum values
  Not_An_Association,                    // An unexpected element

  A_Pragma_Argument_Association,         // 2.8
  A_Discriminant_Association,            // 3.7.1
  A_Record_Component_Association,        // 4.3.1
  An_Array_Component_Association,        // 4.3.3
  A_Parameter_Association,               // 6.4
  A_Generic_Association                  // 12.3
#endif

     string s;

     switch (x)
        {
          case Not_An_Association:             s = "Not_An_Association";             break;
          case A_Pragma_Argument_Association:  s = "A_Pragma_Argument_Association";  break;
          case A_Discriminant_Association:     s = "A_Discriminant_Association";     break;
          case A_Record_Component_Association: s = "A_Record_Component_Association"; break;
          case An_Array_Component_Association: s = "An_Array_Component_Association"; break;
          case A_Parameter_Association:        s = "A_Parameter_Association";        break;
          case A_Generic_Association:          s = "A_Generic_Association";          break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::associationKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }




string
Ada_ROSE_Translation::pathKindName (Path_Kinds x)
   {
#if 0
  // Enum values
  Not_A_Path,
  //  An unexpected element

  //  Statement paths:
  An_If_Path,
  //  5.3:
  //  if condition then
  //    sequence_of_statements

  An_Elsif_Path,
  //  5.3:
  //  elsif condition then
  //    sequence_of_statements

  An_Else_Path,
  //  5.3, 9.7.1, 9.7.3:
  //  else sequence_of_statements

  A_Case_Path,
  //  5.4:
  //  when discrete_choice_list =>
  //    sequence_of_statements

  A_Select_Path,
  //  9.7.1:
  //     select [guard] select_alternative
  //  9.7.2, 9.7.3:
  //     select entry_call_alternative
  //  9.7.4:
  //     select triggering_alternative

  An_Or_Path,
  //  9.7.1:
  //     or [guard] select_alternative
  //  9.7.2:
  //     or delay_alternative

  A_Then_Abort_Path,
  //  9.7.4
  //     then abort sequence_of_statements

  //  //|A2012 start
  //  Expression paths:
  A_Case_Expression_Path,
  //  ??? (RM 2012)
  //  when expression => expression

  An_If_Expression_Path,
  //  ??? (RM 2012)
  //  if condition then expression

  An_Elsif_Expression_Path,
  //  ??? (RM 2012)
  //  elsif condition then expression

  An_Else_Expression_Path
  //  ??? (RM 2012)
  //  else expression
#endif

     string s;

     switch (x)
        {
          case Not_A_Path:               s = "Not_A_Path";               break;
          case An_If_Path:               s = "An_If_Path";               break;
          case An_Elsif_Path:            s = "An_Elsif_Path";            break;
          case An_Else_Path:             s = "An_Else_Path";             break;
          case A_Case_Path:              s = "A_Case_Path";              break;
          case A_Select_Path:            s = "A_Select_Path";            break;
          case An_Or_Path:               s = "An_Or_Path";               break;
          case A_Then_Abort_Path:        s = "A_Then_Abort_Path";        break;
          case A_Case_Expression_Path:   s = "A_Case_Expression_Path";   break;
          case An_If_Expression_Path:    s = "An_If_Expression_Path";    break;
          case An_Elsif_Expression_Path: s = "An_Elsif_Expression_Path"; break;
          case An_Else_Expression_Path:  s = "An_Else_Expression_Path";  break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::pathKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }



string
Ada_ROSE_Translation::operatorKindName (Operator_Kinds x)
   {
#if 0
  // Enum values
     Not_An_Operator,                   // An unexpected element
     An_And_Operator,                   // and
     An_Or_Operator,                    // or
     An_Xor_Operator,                   // xor
     An_Equal_Operator,                 // =
     A_Not_Equal_Operator,              // /=
     A_Less_Than_Operator,              // <
     A_Less_Than_Or_Equal_Operator,     // <=
     A_Greater_Than_Operator,           // >
     A_Greater_Than_Or_Equal_Operator,  // >=
     A_Plus_Operator,                   // +
     A_Minus_Operator,                  // -
     A_Concatenate_Operator,            // &
     A_Unary_Plus_Operator,             // +
     A_Unary_Minus_Operator,            // -
     A_Multiply_Operator,               // *
     A_Divide_Operator,                 // /
     A_Mod_Operator,                    // mod
     A_Rem_Operator,                    // rem
     An_Exponentiate_Operator,          // **
     An_Abs_Operator,                   // abs
     A_Not_Operator                     // not
#endif

     string s;

     switch (x)
        {
          case Not_An_Operator:                  s = "Not_An_Operator";                  break;
          case An_And_Operator:                  s = "An_And_Operator";                  break;
          case An_Or_Operator:                   s = "An_Or_Operator";                   break;
          case An_Xor_Operator:                  s = "An_Xor_Operator";                  break;
          case An_Equal_Operator:                s = "An_Equal_Operator";                break;
          case A_Not_Equal_Operator:             s = "A_Not_Equal_Operator";             break;
          case A_Less_Than_Operator:             s = "A_Less_Than_Operator";             break;
          case A_Less_Than_Or_Equal_Operator:    s = "A_Less_Than_Or_Equal_Operator";    break;
          case A_Greater_Than_Operator:          s = "A_Greater_Than_Operator";          break;
          case A_Greater_Than_Or_Equal_Operator: s = "A_Greater_Than_Or_Equal_Operator"; break;
          case A_Plus_Operator:                  s = "A_Plus_Operator";                  break;
          case A_Minus_Operator:                 s = "A_Minus_Operator";                 break;
          case A_Concatenate_Operator:           s = "A_Concatenate_Operator";           break;
          case A_Unary_Plus_Operator:            s = "A_Unary_Plus_Operator";            break;
          case A_Unary_Minus_Operator:           s = "A_Unary_Minus_Operator";           break;
          case A_Multiply_Operator:              s = "A_Multiply_Operator";              break;
          case A_Divide_Operator:                s = "A_Divide_Operator";                break;
          case A_Mod_Operator:                   s = "A_Mod_Operator";                   break;
          case A_Rem_Operator:                   s = "A_Rem_Operator";                   break;
          case An_Exponentiate_Operator:         s = "An_Exponentiate_Operator";         break;
          case An_Abs_Operator:                  s = "An_Abs_Operator";                  break;
          case A_Not_Operator:                   s = "A_Not_Operator";                   break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::operatorKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


  // DQ (10/12/2017): Required for new update from Charles.
string
Ada_ROSE_Translation::typeKindName (Type_Kinds x)
   {
#if 0
  Not_A_Type_Definition,                 // An unexpected element
  A_Derived_Type_Definition,             // 3.4(2)     -> Trait_Kinds
  A_Derived_Record_Extension_Definition, // 3.4(2)     -> Trait_Kinds
  An_Enumeration_Type_Definition,        // 3.5.1(2)
  A_Signed_Integer_Type_Definition,      // 3.5.4(3)
  A_Modular_Type_Definition,             // 3.5.4(4)
  A_Root_Type_Definition,                // 3.5.4(14), 3.5.6(3)
  //                                               -> Root_Type_Kinds
  A_Floating_Point_Definition,           // 3.5.7(2)
  An_Ordinary_Fixed_Point_Definition,    // 3.5.9(3)
  A_Decimal_Fixed_Point_Definition,      // 3.5.9(6)
  An_Unconstrained_Array_Definition,     // 3.6(2)
  A_Constrained_Array_Definition,        // 3.6(2)
  A_Record_Type_Definition,              // 3.8(2)     -> Trait_Kinds
  A_Tagged_Record_Type_Definition,       // 3.8(2)     -> Trait_Kinds

  //  //|A2005 start
  An_Interface_Type_Definition,          // 3.9.4      -> Interface_Kinds
  //  //|A2005 end

  An_Access_Type_Definition            // 3.10(2)    -> Access_Type_Kinds
#endif

     string s;

     switch (x)
        {
          case Not_A_Type_Definition:                 s = "Not_A_Type_Definition";                 break;
          case A_Derived_Type_Definition:             s = "A_Derived_Type_Definition";             break;
          case A_Derived_Record_Extension_Definition: s = "A_Derived_Record_Extension_Definition"; break;
          case An_Enumeration_Type_Definition:        s = "An_Enumeration_Type_Definition";        break;
          case A_Signed_Integer_Type_Definition:      s = "A_Signed_Integer_Type_Definition";      break;
          case A_Modular_Type_Definition:             s = "A_Modular_Type_Definition";             break;
          case A_Root_Type_Definition:                s = "A_Root_Type_Definition";                break;
          case A_Floating_Point_Definition:           s = "A_Floating_Point_Definition";           break;
          case An_Ordinary_Fixed_Point_Definition:    s = "An_Ordinary_Fixed_Point_Definition";    break;
          case A_Decimal_Fixed_Point_Definition:      s = "A_Decimal_Fixed_Point_Definition";      break;
          case An_Unconstrained_Array_Definition:     s = "An_Unconstrained_Array_Definition";     break;
          case A_Constrained_Array_Definition:        s = "A_Constrained_Array_Definition";        break;
          case A_Record_Type_Definition:              s = "A_Record_Type_Definition";              break;
          case A_Tagged_Record_Type_Definition:       s = "A_Tagged_Record_Type_Definition";       break;
          case An_Interface_Type_Definition:          s = "An_Interface_Type_Definition";          break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::typeKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::declarationOriginName (Declaration_Origins x)
   {
#if 0
  // Enum values
  Not_A_Declaration_Origin,  //  An unexpected element
  An_Explicit_Declaration,   //  3.1(5) explicitly declared in the text of a program, or within an expanded generic template
  An_Implicit_Predefined_Declaration,   //  3.1(5), 3.2.3(1), A.1(2)
  An_Implicit_Inherited_Declaration  //  3.1(5), 3.4(6-35)
#endif

     string s;

     switch (x)
        {
          case Not_A_Declaration_Origin:           s = "Not_A_Declaration_Origin";           break;
          case An_Explicit_Declaration:            s = "An_Explicit_Declaration";            break;
          case An_Implicit_Predefined_Declaration: s = "An_Implicit_Predefined_Declaration"; break;
          case An_Implicit_Inherited_Declaration:  s = "An_Implicit_Inherited_Declaration";  break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::declarationOriginName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::modeKindName (Mode_Kinds x)
   {
#if 0
  // Enum values
  Not_A_Mode,              // An unexpected element
  A_Default_In_Mode,       // procedure A(B :        C);
  An_In_Mode,              // procedure A(B : IN     C);
  An_Out_Mode,             // procedure A(B :    OUT C);
  An_In_Out_Mode           // procedure A(B : IN OUT C);
#endif

     string s;

     switch (x)
        {
          case Not_A_Mode:        s = "Not_A_Mode";        break;
          case A_Default_In_Mode: s = "A_Default_In_Mode"; break;
          case An_In_Mode:        s = "An_In_Mode";        break;
          case An_Out_Mode:       s = "An_Out_Mode";       break;
          case An_In_Out_Mode:    s = "An_In_Out_Mode";    break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::modeKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::subprogramDefaultKindName (Subprogram_Default_Kinds x)
   {
#if 0
  // Enum values
     Not_A_Default,         // An unexpected element
     A_Name_Default,        // with subprogram_specification is default_name;
     A_Box_Default,         // with subprogram_specification is <>;
//  //|A2005 start
     A_Null_Default,        // with subprogram_specification is null;
//  //|A2005 end
     A_Nil_Default          // with subprogram_specification;
#endif

     string s;

     switch (x)
        {
          case Not_A_Default:  s = "Not_A_Default";  break;
          case A_Name_Default: s = "A_Name_Default"; break;
          case A_Box_Default:  s = "A_Box_Default";  break;
          case A_Null_Default: s = "A_Null_Default"; break;
          case A_Nil_Default:  s = "A_Nil_Default";  break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::subprogramDefaultKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


#if 0
string
Ada_ROSE_Translation::traitKindName (Trait_Kinds x)
   {
#if 0
  // Enum values
     Not_A_Trait,                         // An unexpected element

     An_Ordinary_Trait,
  //  The declaration or definition does not contain the reserved words
  //  "aliased", "reverse", "private", "limited", "abstract", or "access"
  //  in an access_definition

     An_Aliased_Trait,
  //  "aliased" is present

     An_Access_Definition_Trait,
  //  "access" in an access_definition is present
  //  //|A2005 start
     A_Null_Exclusion_Trait,
  //  "not null" is present
  //  //|A2005 end
     A_Reverse_Trait,
  //  "reverse" is present
     A_Private_Trait,
  //  Only "private" is present
     A_Limited_Trait,
  //  Only "limited" is present
     A_Limited_Private_Trait,
  //  "limited" and "private" are present
     An_Abstract_Trait,
  //  Only "abstract" is present
     An_Abstract_Private_Trait,
  //  "abstract" and "private" are present
     An_Abstract_Limited_Trait,
  //  "abstract" and "limited" are present
     An_Abstract_Limited_Private_Trait
  //  "abstract", "limited", and "private" are present

  //  //|D2005 start
  //  We need a note saying that An_Access_Definition_Trait is an obsolescent
  //  value kept only because of upward compatibility reasons. Now an
  //  access_definition that defines an anonymous access kind is represented as
  //  a first-class citizen in the ASIS Element classification hierarchy
  //  (An_Access_Definition value in Definition_Kinds and the subordinate
  //  Access_Definition_Kinds type).
  //  //|D2005 end
#endif

     string s;

     switch (x)
        {
          case Not_A_Trait:                       s = "Not_A_Trait";                       break;
          case An_Ordinary_Trait:                 s = "An_Ordinary_Trait";                 break;
          case An_Aliased_Trait:                  s = "An_Aliased_Trait";                  break;
          case An_Access_Definition_Trait:        s = "An_Access_Definition_Trait";        break;
          case A_Null_Exclusion_Trait:            s = "A_Null_Exclusion_Trait";            break;
          case A_Reverse_Trait:                   s = "A_Reverse_Trait";                   break;
          case A_Private_Trait:                   s = "A_Private_Trait";                   break;
          case A_Limited_Trait:                   s = "A_Limited_Trait";                   break;
          case A_Limited_Private_Trait:           s = "A_Limited_Private_Trait";           break;
          case An_Abstract_Trait:                 s = "An_Abstract_Trait";                 break;
          case An_Abstract_Private_Trait:         s = "An_Abstract_Private_Trait";         break;
          case An_Abstract_Limited_Trait:         s = "An_Abstract_Limited_Trait";         break;
          case An_Abstract_Limited_Private_Trait: s = "An_Abstract_Limited_Private_Trait"; break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::traitKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }
#endif


string
Ada_ROSE_Translation::attributeKindName (Attribute_Kinds x)
   {
#if 0
  // Enum values
  Not_An_Attribute,              // An unexpected element

  An_Access_Attribute,           // 3.10.2(24), 3.10.2(32), K(2), K(4)
  An_Address_Attribute,          // 13.3(11), J.7.1(5), K(6)
  An_Adjacent_Attribute,         // A.5.3(48), K(8)
  An_Aft_Attribute,              // 3.5.10(5), K(12)
  An_Alignment_Attribute,        // 13.3(23), K(14)
  A_Base_Attribute,              // 3.5(15), K(17)
  A_Bit_Order_Attribute,         // 13.5.3(4), K(19)
  A_Body_Version_Attribute,      // E.3(4), K(21)
  A_Callable_Attribute,          // 9.9(2), K(23)
  A_Caller_Attribute,            // C.7.1(14), K(25)
  A_Ceiling_Attribute,           // A.5.3(33), K(27)
  A_Class_Attribute,             // 3.9(14), 7.3.1(9), K(31), K(34)
  A_Component_Size_Attribute,    // 13.3(69), K(36)
  A_Compose_Attribute,           // A.5.3(24), K(38)
  A_Constrained_Attribute,       // 3.7.2(3), J.4(2), K(42)
  A_Copy_Sign_Attribute,         // A.5.3(51), K(44)
  A_Count_Attribute,             // 9.9(5), K(48)
  A_Definite_Attribute,          // 12.5.1(23), K(50)
  A_Delta_Attribute,             // 3.5.10(3), K(52)
  A_Denorm_Attribute,            // A.5.3(9), K(54)
  A_Digits_Attribute,            // 3.5.8(2), 3.5.10(7), K(56), K(58)

  An_Exponent_Attribute,         // A.5.3(18), K(60)
  An_External_Tag_Attribute,     // 13.3(75), K(64)
  A_First_Attribute,             // 3.5(12), 3.6.2(3), K(68), K(70)
  A_First_Bit_Attribute,         // 13.5.2(3), K(72)
  A_Floor_Attribute,             // A.5.3(30), K(74)
  A_Fore_Attribute,              // 3.5.10(4), K(78)
  A_Fraction_Attribute,          // A.5.3(21), K(80)
  An_Identity_Attribute,         // 11.4.1(9), C.7.1(12), K(84), K(86)
  An_Image_Attribute,            // 3.5(35), K(88)
  An_Input_Attribute,            // 13.13.2(22), 13.13.2(32), K(92), K(96)
  A_Last_Attribute,              // 3.5(13), 3.6.2(5), K(102), K(104)
  A_Last_Bit_Attribute,          // 13.5.2(4), K(106)
  A_Leading_Part_Attribute,      // A.5.3(54), K(108)
  A_Length_Attribute,            // 3.6.2(9), K(117)
  A_Machine_Attribute,           // A.5.3(60), K(119)
  A_Machine_Emax_Attribute,      // A.5.3(8), K(123)
  A_Machine_Emin_Attribute,      // A.5.3(7), K(125)
  A_Machine_Mantissa_Attribute,  // A.5.3(6), K(127)
  A_Machine_Overflows_Attribute, // A.5.3(12), A.5.4(4), K(129), K(131)
  A_Machine_Radix_Attribute,     // A.5.3(2), A.5.4(2), K(133), K(135)
  A_Machine_Rounds_Attribute,    // A.5.3(11), A.5.4(3), K(137), K(139)
  A_Max_Attribute,               // 3.5(19), K(141)
  A_Max_Size_In_Storage_Elements_Attribute, //   13.11.1(3), K(145)
  A_Min_Attribute,               // 3.5(16), K(147)
  A_Model_Attribute,             // A.5.3(68), G.2.2(7), K(151)
  A_Model_Emin_Attribute,        // A.5.3(65), G.2.2(4), K(155)
  A_Model_Epsilon_Attribute,     // A.5.3(66), K(157)
  A_Model_Mantissa_Attribute,    // A.5.3(64), G.2.2(3), K(159)
  A_Model_Small_Attribute,       // A.5.3(67), K(161)
  A_Modulus_Attribute,           // 3.5.4(17), K(163)

  An_Output_Attribute,           // 13.13.2(19), 13.13.2(29), K(165), K(169)
  A_Partition_ID_Attribute,      // E.1(9), K(173)
  A_Pos_Attribute,               // 3.5.5(2), K(175)
  A_Position_Attribute,          // 13.5.2(2), K(179)
  A_Pred_Attribute,              // 3.5(25), K(181)
  A_Range_Attribute,             // 3.5(14), 3.6.2(7), K(187), ú(189)
  A_Read_Attribute,              // 13.13.2(6), 13.13.2(14), K(191), K(195)
  A_Remainder_Attribute,         // A.5.3(45), K(199)
  A_Round_Attribute,             // 3.5.10(12), K(203)
  A_Rounding_Attribute,          // A.5.3(36), K(207)
  A_Safe_First_Attribute,        // A.5.3(71), G.2.2(5), K(211)
  A_Safe_Last_Attribute,         // A.5.3(72), G.2.2(6), K(213)
  A_Scale_Attribute,             // 3.5.10(11), K(215)
  A_Scaling_Attribute,           // A.5.3(27), K(217)
  A_Signed_Zeros_Attribute,      // A.5.3(13), K(221)
  A_Size_Attribute,              // 13.3(40), 13.3(45), K(223), K(228)
  A_Small_Attribute,             // 3.5.10(2), K(230)
  A_Storage_Pool_Attribute,      // 13.11(13), K(232)
  A_Storage_Size_Attribute,      // 13.3(60), 13.11(14), J.9(2), K(234),
  //                                 K(236)
  A_Succ_Attribute,              // 3.5(22), K(238)
  A_Tag_Attribute,               // 3.9(16), 3.9(18), K(242), K(244)
  A_Terminated_Attribute,        // 9.9(3), K(246)
  A_Truncation_Attribute,        // A.5.3(42), K(248)
  An_Unbiased_Rounding_Attribute, // A.5.3(39), K(252)
  An_Unchecked_Access_Attribute,  // 13.10(3), H.4(18), K(256)
  A_Val_Attribute,                // 3.5.5(5), K(258)
  A_Valid_Attribute,              // 13.9.2(3), H(6), K(262)
  A_Value_Attribute,              // 3.5(52), K(264)
  A_Version_Attribute,            // E.3(3), K(268)
  A_Wide_Image_Attribute,         // 3.5(28), K(270)
  A_Wide_Value_Attribute,         // 3.5(40), K(274)
  A_Wide_Width_Attribute,         // 3.5(38), K(278)
  A_Width_Attribute,              // 3.5(39), K(280)
  A_Write_Attribute,              // 13.13.2(3), 13.13.2(11), K(282), K(286)

  //  |A2005 start
  //  New Ada 2005 attributes. To be alphabetically ordered later
  A_Machine_Rounding_Attribute,
  A_Mod_Attribute,
  A_Priority_Attribute,
  A_Stream_Size_Attribute,
  A_Wide_Wide_Image_Attribute,
  A_Wide_Wide_Value_Attribute,
  A_Wide_Wide_Width_Attribute,
  //  |A2005 end

  //  |A2012 start
  //  New Ada 2012 attributes. To be alphabetically ordered later
  A_Max_Alignment_For_Allocation_Attribute,
  An_Overlaps_Storage_Attribute,
  //  |A2012 end

  An_Implementation_Defined_Attribute,  // Reference Manual, Annex M
  An_Unknown_Attribute           // Unknown to ASIS
#endif

     string s;

     switch (x)
        {
          case Not_An_Attribute:                         s = "Not_An_Attribute"; break;
          case An_Access_Attribute:                      s = "An_Access_Attribute"; break;
          case An_Address_Attribute:                     s = "An_Address_Attribute"; break;
          case An_Adjacent_Attribute:                    s = "An_Adjacent_Attribute"; break;
          case An_Aft_Attribute:                         s = "An_Aft_Attribute"; break;
          case An_Alignment_Attribute:                   s = "An_Alignment_Attribute"; break;
          case A_Base_Attribute:                         s = "A_Base_Attribute"; break;
          case A_Bit_Order_Attribute:                    s = "A_Bit_Order_Attribute"; break;
          case A_Body_Version_Attribute:                 s = "A_Body_Version_Attribute"; break;
          case A_Callable_Attribute:                     s = "A_Callable_Attribute"; break;
          case A_Caller_Attribute:                       s = "A_Caller_Attribute"; break;
          case A_Ceiling_Attribute:                      s = "A_Ceiling_Attribute"; break;
          case A_Class_Attribute:                        s = "A_Class_Attribute"; break;
          case A_Component_Size_Attribute:               s = "A_Component_Size_Attribute"; break;
          case A_Compose_Attribute:                      s = "A_Compose_Attribute"; break;
          case A_Constrained_Attribute:                  s = "A_Constrained_Attribute"; break;
          case A_Copy_Sign_Attribute:                    s = "A_Copy_Sign_Attribute"; break;
          case A_Count_Attribute:                        s = "A_Count_Attribute"; break;
          case A_Definite_Attribute:                     s = "A_Definite_Attribute"; break;
          case A_Delta_Attribute:                        s = "A_Delta_Attribute"; break;
          case A_Denorm_Attribute:                       s = "A_Denorm_Attribute"; break;
          case A_Digits_Attribute:                       s = "A_Digits_Attribute"; break;
          case An_Exponent_Attribute:                    s = "An_Exponent_Attribute"; break;
          case An_External_Tag_Attribute:                s = "An_External_Tag_Attribute"; break;
          case A_First_Attribute:                        s = "A_First_Attribute"; break;
          case A_First_Bit_Attribute:                    s = "A_First_Bit_Attribute"; break;
          case A_Floor_Attribute:                        s = "A_Floor_Attribute"; break;
          case A_Fore_Attribute:                         s = "A_Fore_Attribute"; break;
          case A_Fraction_Attribute:                     s = "A_Fraction_Attribute"; break;
          case An_Identity_Attribute:                    s = "An_Identity_Attribute"; break;
          case An_Image_Attribute:                       s = "An_Image_Attribute"; break;
          case An_Input_Attribute:                       s = "An_Input_Attribute"; break;
          case A_Last_Attribute:                         s = "A_Last_Attribute"; break;
          case A_Last_Bit_Attribute:                     s = "A_Last_Bit_Attribute"; break;
          case A_Leading_Part_Attribute:                 s = "A_Leading_Part_Attribute"; break;
          case A_Length_Attribute:                       s = "A_Length_Attribute"; break;
          case A_Machine_Attribute:                      s = "A_Machine_Attribute"; break;
          case A_Machine_Emax_Attribute:                 s = "A_Machine_Emax_Attribute"; break;
          case A_Machine_Emin_Attribute:                 s = "A_Machine_Emin_Attribute"; break;
          case A_Machine_Mantissa_Attribute:             s = "A_Machine_Mantissa_Attribute"; break;
          case A_Machine_Overflows_Attribute:            s = "A_Machine_Overflows_Attribute"; break;
          case A_Machine_Radix_Attribute:                s = "A_Machine_Radix_Attribute"; break;
          case A_Machine_Rounds_Attribute:               s = "A_Machine_Rounds_Attribute"; break;
          case A_Max_Attribute:                          s = "A_Max_Attribute"; break;
          case A_Max_Size_In_Storage_Elements_Attribute: s = "A_Max_Size_In_Storage_Elements_Attribute"; break;
          case A_Min_Attribute:                          s = "A_Min_Attribute"; break;
          case A_Model_Attribute:                        s = "A_Model_Attribute"; break;
          case A_Model_Emin_Attribute:                   s = "A_Model_Emin_Attribute"; break;
          case A_Model_Epsilon_Attribute:                s = "A_Model_Epsilon_Attribute"; break;
          case A_Model_Mantissa_Attribute:               s = "A_Model_Mantissa_Attribute"; break;
          case A_Model_Small_Attribute:                  s = "A_Model_Small_Attribute"; break;
          case A_Modulus_Attribute:                      s = "A_Modulus_Attribute"; break;
          case An_Output_Attribute:                      s = "An_Output_Attribute"; break;
          case A_Partition_ID_Attribute:                 s = "A_Partition_ID_Attribute"; break;
          case A_Pos_Attribute:                          s = "A_Pos_Attribute"; break;
          case A_Position_Attribute:                     s = "A_Position_Attribute"; break;
          case A_Pred_Attribute:                         s = "A_Pred_Attribute"; break;
          case A_Range_Attribute:                        s = "A_Range_Attribute"; break;
          case A_Read_Attribute:                         s = "A_Read_Attribute"; break;
          case A_Remainder_Attribute:                    s = "A_Remainder_Attribute"; break;
          case A_Round_Attribute:                        s = "A_Round_Attribute"; break;
          case A_Rounding_Attribute:                     s = "A_Rounding_Attribute"; break;
          case A_Safe_First_Attribute:                   s = "A_Safe_First_Attribute"; break;
          case A_Safe_Last_Attribute:                    s = "A_Safe_Last_Attribute"; break;
          case A_Scale_Attribute:                        s = "A_Scale_Attribute"; break;
          case A_Scaling_Attribute:                      s = "A_Scaling_Attribute"; break;
          case A_Signed_Zeros_Attribute:                 s = "A_Signed_Zeros_Attribute"; break;
          case A_Size_Attribute:                         s = "A_Size_Attribute"; break;
          case A_Small_Attribute:                        s = "A_Small_Attribute"; break;
          case A_Storage_Pool_Attribute:                 s = "A_Storage_Pool_Attribute"; break;
          case A_Storage_Size_Attribute:                 s = "A_Storage_Size_Attribute"; break;
          case A_Succ_Attribute:                         s = "A_Succ_Attribute"; break;
          case A_Tag_Attribute:                          s = "A_Tag_Attribute"; break;
          case A_Terminated_Attribute:                   s = "A_Terminated_Attribute"; break;
          case A_Truncation_Attribute:                   s = "A_Truncation_Attribute"; break;
          case An_Unbiased_Rounding_Attribute:           s = "An_Unbiased_Rounding_Attribute"; break;
          case An_Unchecked_Access_Attribute:            s = "An_Unchecked_Access_Attribute"; break;
          case A_Val_Attribute:                          s = "A_Val_Attribute"; break;
          case A_Valid_Attribute:                        s = "A_Valid_Attribute"; break;
          case A_Value_Attribute:                        s = "A_Value_Attribute"; break;
          case A_Version_Attribute:                      s = "A_Version_Attribute"; break;
          case A_Wide_Image_Attribute:                   s = "A_Wide_Image_Attribute"; break;
          case A_Wide_Value_Attribute:                   s = "A_Wide_Value_Attribute"; break;
          case A_Wide_Width_Attribute:                   s = "A_Wide_Width_Attribute"; break;
          case A_Width_Attribute:                        s = "A_Width_Attribute"; break;
          case A_Write_Attribute:                        s = "A_Write_Attribute"; break;
          case A_Machine_Rounding_Attribute:             s = "A_Machine_Rounding_Attribute"; break;
          case A_Mod_Attribute:                          s = "A_Mod_Attribute"; break;
          case A_Priority_Attribute:                     s = "A_Priority_Attribute"; break;
          case A_Stream_Size_Attribute:                  s = "A_Stream_Size_Attribute"; break;
          case A_Wide_Wide_Image_Attribute:              s = "A_Wide_Wide_Image_Attribute"; break;
          case A_Wide_Wide_Value_Attribute:              s = "A_Wide_Wide_Value_Attribute"; break;
          case A_Wide_Wide_Width_Attribute:              s = "A_Wide_Wide_Width_Attribute"; break;
          case A_Max_Alignment_For_Allocation_Attribute: s = "A_Max_Alignment_For_Allocation_Attribute"; break;
          case An_Overlaps_Storage_Attribute:            s = "An_Overlaps_Storage_Attribute"; break;
          case An_Implementation_Defined_Attribute:      s = "An_Implementation_Defined_Attribute"; break;
          case An_Unknown_Attribute:                     s = "An_Unknown_Attribute"; break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::attributeKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::unitKindName (Unit_Kinds x)
   {
#if 0
  // Enum values
  Not_A_Unit,

  A_Procedure,
  A_Function,
  A_Package,

  A_Generic_Procedure,
  A_Generic_Function,
  A_Generic_Package,

  A_Procedure_Instance,
  A_Function_Instance,
  A_Package_Instance,

  A_Procedure_Renaming,
  A_Function_Renaming,
  A_Package_Renaming,

  A_Generic_Procedure_Renaming,
  A_Generic_Function_Renaming,
  A_Generic_Package_Renaming,

  A_Procedure_Body,
  //  A unit interpreted only as the completion of a procedure, or a unit
  //  interpreted as both the declaration and body of a library
  //  procedure. Reference Manual 10.1.4(4)
  A_Function_Body,
  //  A unit interpreted only as the completion of a function, or a unit
  //  interpreted as both the declaration and body of a library
  //  function. Reference Manual 10.1.4(4)
  A_Package_Body,

  A_Procedure_Body_Subunit,
  A_Function_Body_Subunit,
  A_Package_Body_Subunit,
  A_Task_Body_Subunit,
  A_Protected_Body_Subunit,

  A_Nonexistent_Declaration,
  //  A unit that does not exist but is:
  //    1) mentioned in a with clause of another unit or,
  //    2) a required corresponding library_unit_declaration
  A_Nonexistent_Body,
  //  A unit that does not exist but is:
  //     1) known to be a corresponding subunit or,
  //     2) a required corresponding library_unit_body
  A_Configuration_Compilation,
  //  Corresponds to the whole content of a compilation with no
  //  compilation_unit, but possibly containing comments, configuration
  //  pragmas, or both. Any Context can have at most one unit of
  //  A_Configuration_Compilation kind. A unit of
  //  A_Configuration_Compilation does not have a name. This unit
  //  represents configuration pragmas that are "in effect".
  //
  //  GNAT-specific note: In case of GNAT the requirement to have at most
  //  one unit of A_Configuration_Compilation kind does not make sense: in
  //  GNAT compilation model configuration pragmas are contained in
  //  configuration files, and a compilation may use an arbitrary number
  //  of configuration files. That is, (Elements representing) different
  //  configuration pragmas may have different enclosing compilation units
  //  with different text names. So in the ASIS implementation for GNAT a
  //  Context may contain any number of units of
  //  A_Configuration_Compilation kind
  An_Unknown_Unit
#endif

     string s;

     switch (x)
        {
          case Not_A_Unit:                   s = "Not_A_Unit";                   break;
          case A_Procedure:                  s = "A_Procedure";                  break;
          case A_Function:                   s = "A_Function";                   break;
          case A_Package:                    s = "A_Package";                    break;
          case A_Generic_Procedure:          s = "A_Generic_Procedure";          break;
          case A_Generic_Function:           s = "A_Generic_Function";           break;
          case A_Generic_Package:            s = "A_Generic_Package";            break;
          case A_Procedure_Instance:         s = "A_Procedure_Instance";         break;
          case A_Function_Instance:          s = "A_Function_Instance";          break;
          case A_Package_Instance:           s = "A_Package_Instance";           break;
          case A_Procedure_Renaming:         s = "A_Procedure_Renaming";         break;
          case A_Function_Renaming:          s = "A_Function_Renaming";          break;
          case A_Package_Renaming:           s = "A_Package_Renaming";           break;
          case A_Generic_Procedure_Renaming: s = "A_Generic_Procedure_Renaming"; break;
          case A_Generic_Function_Renaming:  s = "A_Generic_Function_Renaming";  break;
          case A_Generic_Package_Renaming:   s = "A_Generic_Package_Renaming";   break;
          case A_Procedure_Body:             s = "A_Procedure_Body";             break;
          case A_Function_Body:              s = "A_Function_Body";              break;
          case A_Package_Body:               s = "A_Package_Body";               break;
          case A_Procedure_Body_Subunit:     s = "A_Procedure_Body_Subunit";     break;
          case A_Function_Body_Subunit:      s = "A_Function_Body_Subunit";      break;
          case A_Package_Body_Subunit:       s = "A_Package_Body_Subunit";       break;
          case A_Task_Body_Subunit:          s = "A_Task_Body_Subunit";          break;
          case A_Protected_Body_Subunit:     s = "A_Protected_Body_Subunit";     break;
          case A_Nonexistent_Declaration:    s = "A_Nonexistent_Declaration";    break;
          case A_Nonexistent_Body:           s = "A_Nonexistent_Body";           break;
          case A_Configuration_Compilation:  s = "A_Configuration_Compilation";  break;
          case An_Unknown_Unit:              s = "An_Unknown_Unit";              break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::unitKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::unitClassName (Unit_Classes x)
   {
#if 0
  // Enum values
  Not_A_Class,
  //  A nil, nonexistent, unknown, or configuration compilation unit class.
  A_Public_Declaration,
  //  library_unit_declaration or library_unit_renaming_declaration.
  A_Public_Body,
  //  library_unit_body interpreted only as a completion. Its declaration
  //  is public.
  A_Public_Declaration_And_Body,
  //  subprogram_body interpreted as both a declaration and body of a
  //  library subprogram - Reference Manual 10.1.4(4).
  A_Private_Declaration,
  //  private library_unit_declaration or private
  //  library_unit_renaming_declaration.
  A_Private_Body,
  //  library_unit_body interpreted only as a completion. Its declaration
  //  is private.
  A_Separate_Body
  //  separate (parent_unit_name) proper_body.
#endif

     string s;

     switch (x)
        {
          case Not_A_Class:                   s = "Not_A_Class";                   break;
          case A_Public_Declaration:          s = "A_Public_Declaration";          break;
          case A_Public_Body:                 s = "A_Public_Body";                 break;
          case A_Public_Declaration_And_Body: s = "A_Public_Declaration_And_Body"; break;
          case A_Private_Declaration:         s = "A_Private_Declaration";         break;
          case A_Private_Body:                s = "A_Private_Body";                break;
          case A_Separate_Body:               s = "A_Separate_Body";               break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::unitClassName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::unitOriginName (Unit_Origins x)
   {
#if 0
  // Enum values
  Not_An_Origin,
  //  A nil or nonexistent unit origin. An_Unknown_Unit can be any origin
  A_Predefined_Unit,
  //  Ada predefined language environment units listed in Annex A(2).
  //  These include Standard and the three root library units: Ada,
  //  Interfaces, and System, and their descendants.  i.e., Ada.Text_Io,
  //  Ada.Calendar, Interfaces.C, etc.
  An_Implementation_Unit,
  //  Implementation specific library units, e.g., runtime support
  //  packages, utility libraries, etc. It is not required that any
  //  implementation supplied units have this origin. This is a suggestion.
  //  Implementations might provide, for example, precompiled versions of
  //  public domain software that could have An_Application_Unit origin.
  An_Application_Unit
  //  Neither A_Predefined_Unit or An_Implementation_Unit
#endif

     string s;

     switch (x)
        {
          case Not_An_Origin:          s = "Not_An_Origin";          break;
          case A_Predefined_Unit:      s = "A_Predefined_Unit";      break;
          case An_Implementation_Unit: s = "An_Implementation_Unit"; break;
          case An_Application_Unit:    s = "An_Application_Unit";    break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::unitOriginName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::rootTypeKindName (Root_Type_Kinds x)
   {
     string s;

     switch (x)
        {
          case Not_A_Root_Type_Definition:     s = "Not_A_Root_Type_Definition";     break;
          case A_Root_Integer_Definition:      s = "A_Root_Integer_Definition";      break;
          case A_Root_Real_Definition:         s = "A_Root_Real_Definition";         break;
          case A_Universal_Integer_Definition: s = "A_Universal_Integer_Definition"; break;
          case A_Universal_Real_Definition:    s = "A_Universal_Real_Definition";    break;
          case A_Universal_Fixed_Definition:   s = "A_Universal_Fixed_Definition";   break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::rootTypeKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::interfaceKindName (Interface_Kinds x)
   {
#if 0
  Not_An_Interface,                 // An unexpected element
  An_Ordinary_Interface,            // interface ...
  A_Limited_Interface,              // limited interface ...
  A_Task_Interface,                 // task interface ...
  A_Protected_Interface,            // protected interface ...
  A_Synchronized_Interface        // synchronized interface ...
#endif

     string s;

     switch (x)
        {
          case Not_An_Interface:         s = "Not_An_Interface";         break;
          case An_Ordinary_Interface:    s = "An_Ordinary_Interface";    break;
          case A_Limited_Interface:      s = "A_Limited_Interface";      break;
          case A_Task_Interface:         s = "A_Task_Interface";         break;
          case A_Protected_Interface:    s = "A_Protected_Interface";    break;
          case A_Synchronized_Interface: s = "A_Synchronized_Interface"; break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::interfaceKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::accessTypeKindName (Access_Type_Kinds x)
   {
#if 0
  Not_An_Access_Type_Definition,       // An unexpected element
  A_Pool_Specific_Access_To_Variable,  // access subtype_indication
  An_Access_To_Variable,               // access all subtype_indication
  An_Access_To_Constant,               // access constant subtype_indication
  An_Access_To_Procedure,              // access procedure
  An_Access_To_Protected_Procedure,    // access protected procedure
  An_Access_To_Function,               // access function
  An_Access_To_Protected_Function      // access protected function
#endif

     string s;

     switch (x)
        {
          case Not_An_Access_Type_Definition:      s = "Not_An_Access_Type_Definition"; break;
          case A_Pool_Specific_Access_To_Variable: s = "A_Pool_Specific_Access_To_Variable"; break;
          case An_Access_To_Variable:              s = "An_Access_To_Variable"; break;
          case An_Access_To_Constant:              s = "An_Access_To_Constant"; break;
          case An_Access_To_Procedure:             s = "An_Access_To_Procedure"; break;
          case An_Access_To_Protected_Procedure:   s = "An_Access_To_Protected_Procedure"; break;
          case An_Access_To_Function:              s = "An_Access_To_Function"; break;
          case An_Access_To_Protected_Function:    s = "An_Access_To_Protected_Function"; break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::accessTypeKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::accessDefinitionKindName (Access_Definition_Kinds x)
   {
#if 0
typedef enum _Access_Definition_Kinds {
  Not_An_Access_Definition,       // An unexpected element
  An_Anonymous_Access_To_Variable,  // [...] access subtype_mark
  An_Anonymous_Access_To_Constant,  // [...] access constant subtype_mark
  An_Anonymous_Access_To_Procedure,           // access procedure
  An_Anonymous_Access_To_Protected_Procedure, // access protected procedure
  An_Anonymous_Access_To_Function,            // access function
  An_Anonymous_Access_To_Protected_Function   // access protected function
} Access_Definition_Kinds;
#endif

     string s;

     switch (x)
        {
          case Not_An_Access_Definition:                   s = "Not_An_Access_Definition";                   break;
          case An_Anonymous_Access_To_Variable:            s = "An_Anonymous_Access_To_Variable";            break;
          case An_Anonymous_Access_To_Constant:            s = "An_Anonymous_Access_To_Constant";            break;
          case An_Anonymous_Access_To_Procedure:           s = "An_Anonymous_Access_To_Procedure";           break;
          case An_Anonymous_Access_To_Protected_Procedure: s = "An_Anonymous_Access_To_Protected_Procedure"; break;
          case An_Anonymous_Access_To_Function:            s = "An_Anonymous_Access_To_Function";            break;
          case An_Anonymous_Access_To_Protected_Function:  s = "An_Anonymous_Access_To_Protected_Function";  break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::accessDefinitionKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }


string
Ada_ROSE_Translation::formalTypeKindName (Formal_Type_Kinds x)
   {
#if 0
typedef enum _Formal_Type_Kinds {
  Not_A_Formal_Type_Definition,             // An unexpected element
  A_Formal_Private_Type_Definition,         // 12.5.1(2)   -> Trait_Kinds
  A_Formal_Tagged_Private_Type_Definition,  // 12.5.1(2)   -> Trait_Kinds
  A_Formal_Derived_Type_Definition,         // 12.5.1(3)   -> Trait_Kinds
  A_Formal_Discrete_Type_Definition,        // 12.5.2(2)
  A_Formal_Signed_Integer_Type_Definition,  // 12.5.2(3)
  A_Formal_Modular_Type_Definition,         // 12.5.2(4)
  A_Formal_Floating_Point_Definition,       // 12.5.2(5)
  A_Formal_Ordinary_Fixed_Point_Definition, // 12.5.2(6)
  A_Formal_Decimal_Fixed_Point_Definition,  // 12.5.2(7)

  //|A2005 start
  A_Formal_Interface_Type_Definition,       // 12.5.5(2) -> Interface_Kinds
  //|A2005 end

  A_Formal_Unconstrained_Array_Definition,  // 3.6(3)
  A_Formal_Constrained_Array_Definition,    // 3.6(5)
  A_Formal_Access_Type_Definition           // 3.10(3),3.10(5)
  //                                                 -> Access_Type_Kinds
} Formal_Type_Kinds;
#endif

     string s;

     switch (x)
        {
          case Not_A_Formal_Type_Definition:             s = "Not_A_Formal_Type_Definition";             break;
          case A_Formal_Private_Type_Definition:         s = "A_Formal_Private_Type_Definition";         break;
          case A_Formal_Tagged_Private_Type_Definition:  s = "A_Formal_Tagged_Private_Type_Definition";  break;
          case A_Formal_Derived_Type_Definition:         s = "A_Formal_Derived_Type_Definition";         break;
          case A_Formal_Discrete_Type_Definition:        s = "A_Formal_Discrete_Type_Definition";        break;
          case A_Formal_Signed_Integer_Type_Definition:  s = "A_Formal_Signed_Integer_Type_Definition";  break;
          case A_Formal_Modular_Type_Definition:         s = "A_Formal_Modular_Type_Definition";         break;
          case A_Formal_Floating_Point_Definition:       s = "A_Formal_Floating_Point_Definition";       break;
          case A_Formal_Ordinary_Fixed_Point_Definition: s = "A_Formal_Ordinary_Fixed_Point_Definition"; break;
          case A_Formal_Decimal_Fixed_Point_Definition:  s = "A_Formal_Decimal_Fixed_Point_Definition";  break;
          case A_Formal_Interface_Type_Definition:       s = "A_Formal_Interface_Type_Definition";       break;
          case A_Formal_Unconstrained_Array_Definition:  s = "A_Formal_Unconstrained_Array_Definition";  break;
          case A_Formal_Constrained_Array_Definition:    s = "A_Formal_Constrained_Array_Definition";    break;
          case A_Formal_Access_Type_Definition:          s = "A_Formal_Access_Type_Definition";          break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::formalTypeKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::discreteRangeKindName (Discrete_Range_Kinds x)
   {
#if 0
typedef enum _Discrete_Range_Kinds {
  Not_A_Discrete_Range,                  // An unexpected element
  A_Discrete_Subtype_Indication,         // 3.6.1(6), 3.2.2
  A_Discrete_Range_Attribute_Reference,  // 3.6.1, 3.5
  A_Discrete_Simple_Expression_Range   // 3.6.1, 3.5
} Discrete_Range_Kinds;
#endif

     string s;

     switch (x)
        {
          case Not_A_Discrete_Range:                 s = "Not_A_Discrete_Range";                 break;
          case A_Discrete_Subtype_Indication:        s = "A_Discrete_Subtype_Indication";        break;
          case A_Discrete_Range_Attribute_Reference: s = "A_Discrete_Range_Attribute_Reference"; break;
          case A_Discrete_Simple_Expression_Range:   s = "A_Discrete_Simple_Expression_Range";   break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::discreteRangeKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::constraintKindName (Constraint_Kinds x)
   {
#if 0
typedef enum _Constraint_Kinds {
  Not_A_Constraint,                      // An unexpected element
  A_Range_Attribute_Reference,           // 3.5(2)
  A_Simple_Expression_Range,             // 3.2.2, 3.5(3)
  A_Digits_Constraint,                   // 3.2.2, 3.5.9
  A_Delta_Constraint,                    // 3.2.2, J.3
  An_Index_Constraint,                   // 3.2.2, 3.6.1
  A_Discriminant_Constraint              // 3.2.2
} Constraint_Kinds;
#endif

     string s;

     switch (x)
        {
          case Not_A_Constraint:            s = "Not_A_Constraint";            break;
          case A_Range_Attribute_Reference: s = "A_Range_Attribute_Reference"; break;
          case A_Simple_Expression_Range:   s = "A_Simple_Expression_Range";   break;
          case A_Digits_Constraint:         s = "A_Digits_Constraint";         break;
          case A_Delta_Constraint:          s = "A_Delta_Constraint";          break;
          case An_Index_Constraint:         s = "An_Index_Constraint";         break;
          case A_Discriminant_Constraint:   s = "A_Discriminant_Constraint";   break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::constraintKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }

string
Ada_ROSE_Translation::representationClauseKindName (Representation_Clause_Kinds x)
   {
#if 0
typedef enum _Representation_Clause_Kinds {
      Not_A_Representation_Clause,              // An unexpected element
      An_Attribute_Definition_Clause,           // 13.3
      An_Enumeration_Representation_Clause,     // 13.4
      A_Record_Representation_Clause,           // 13.5.1
      An_At_Clause                              // J.7
  } Representation_Clause_Kinds;
#endif

     string s;

     switch (x)
        {
          case Not_A_Representation_Clause:          s = "Not_A_Representation_Clause";          break;
          case An_Attribute_Definition_Clause:       s = "An_Attribute_Definition_Clause";       break;
          case An_Enumeration_Representation_Clause: s = "An_Enumeration_Representation_Clause"; break;
          case A_Record_Representation_Clause:       s = "A_Record_Representation_Clause";       break;
          case An_At_Clause:                         s = "An_At_Clause";                         break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::representationClauseKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }



#if 0
string
Ada_ROSE_Translation::xKindName (X_Kinds x)
   {
#if 0
#endif

     string s;

     switch (x)
        {
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;
          case x: s = "x"; break;

          default:
             {
               printf ("Error: default called in switch for Ada_ROSE_Translation::xKindName(): x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }
#endif

