
// #include "rose.h"

// DQ (11/13/2017): This is a violation, sage3basic.h must be the first file included.
// #include "rose_config.h"

#include "sage3basic.h"

#include "rose_config.h"

#include "Ada_to_ROSE_translation.h"

#include "Ada_to_ROSE.h"
//~ #include "untypedBuilder.h"

Ada_ROSE_Translation::ASIS_element_id_to_ASIS_MapType Ada_ROSE_Translation::asisMap;


// void Ada_ROSE_Translation::ada_to_ROSE_translation(List_Node_Struct *head_node, SgSourceFile* file)
void
Ada_ROSE_Translation::ada_to_ROSE_translation(Nodes_Struct& head_nodes, SgSourceFile* file)
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
            // This code treats duplicate entries as a warning and skips the redundant entry.
               if (asisMap.find(element_id) == asisMap.end())
                  {
                    //~ logTrace() << "***** adding element " << element_id << std::endl;
                    asisMap[element_id] = &element;
                  }
                 else
                  {
                    logError() << "ERROR: element_id = " << element_id << " already processed (skipping additional instance) \n";
                  }
               ROSE_ASSERT(asisMap.find(element_id) != asisMap.end());
             }
#if 0
          printf ("current_element->Next = %p \n",current_element->Next);
#endif
          current_element = current_element->Next;
        }



     convertAsisToROSE(head_nodes, file);
   }
