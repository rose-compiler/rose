
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
     logInfo() << "Start of initial traversal of Ada IR node data structure" << std::endl;

  // Traverse the C data structure representing the IR nodes in the Ada AST (building the asisMap).
  // struct List_Node_Struct *current_node = NULL;
     Element_Struct_List_Struct *current_element = head_nodes.Elements;

     while (current_element)
        {
           //~ logInfo() << "current_element = " << current_element
                     //~ << "\nInitial traversal: current_element: Next_Count = "
                     //~ << current_element->Next_Count
                     //~ << std::endl;

           // Build the asisMap of Element ids to Element_Struct pointers.
           Element_Struct & element    = current_element->Element;
           Element_ID       element_id = element.ID;

           ROSE_ASSERT(element_id > MAX_NUMBER_OF_UNITS);

        // This code treats duplicate entries as a warning and skips the redundant entry.
           auto emplaced = asisMap.emplace(element_id, &element);

           if (!emplaced.second)
           {
             // element was already inserted
             logError() << "ERROR: element_id = " << element_id << " already processed (skipping additional instance) \n";
           }

           current_element = current_element->Next;
        }

     logInfo() << "Stored " << asisMap.size() << " ASIS elements for translation."
               << std::endl;

     convertAsisToROSE(head_nodes, file);
   }
