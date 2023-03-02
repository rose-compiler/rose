
#include "sage3basic.h"
#include "Ada_to_ROSE_translation.h"
#include "Ada_to_ROSE.h"

namespace Ada_ROSE_Translation
{

ASIS_element_id_to_ASIS_MapType asisMap;

const ASIS_element_id_to_ASIS_MapType&
elemMap()
   {
      return asisMap;
   }

ASIS_element_id_to_ASIS_MapType&
elemMap_update()
   {
      return asisMap;
   }

void
ada_to_ROSE_translation(Nodes_Struct& head_nodes, SgSourceFile* file)
   {
     logInfo() << "Start of initial traversal of Ada IR node data structure" << std::endl;

  // Traverse the C data structure representing the IR nodes in the Ada AST (building the asisMap).
  // struct List_Node_Struct *current_node = NULL;
     Element_Struct_List_Struct *current_element = head_nodes.Elements;

     while (current_element)
        {
        // Build the asisMap of Element ids to Element_Struct pointers.
           Element_Struct& element    = current_element->Element;
           Element_ID      element_id = element.ID;

           ROSE_ASSERT(element_id > MAX_NUMBER_OF_UNITS);

        // This code treats duplicate entries as a warning and skips the redundant entry.
           auto emplaced = asisMap.emplace(element_id, &element);

           if (!emplaced.second)
           {
             // element was already inserted
             logFlaw() << " element_id = " << element_id << " already processed (skipping additional instance) \n";
           }

           current_element = current_element->Next;
        }

     logInfo() << "Stored " << asisMap.size() << " ASIS elements for translation."
               << std::endl;

     convertAsisToROSE(head_nodes, file);
   }
}
