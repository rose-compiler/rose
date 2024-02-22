#ifndef _ADA_TO_ROSE_TRANSLATION
#define _ADA_TO_ROSE_TRANSLATION 1

#include <unordered_map>
#include "a_nodes.h"

struct SgSourceFile;

namespace Ada_ROSE_Translation
   {
     constexpr int UNTYPED_FILE_ID = 0;
     constexpr int MAX_NUMBER_OF_UNITS = 10;

     // default map used in the translation
     template <class KeyType, class SageNode>
     using map_t = std::unordered_map<KeyType, SageNode>;

     // alternatively:
     //~ template <class KeyType, class SageNode>
     //~ using map_t = std::map<KeyType, SageNode>;

     using ASIS_element_id_to_ASIS_MapType = map_t<int, _Element_Struct*> ;

     // node mapping accessors, storage, and retrieval
     // of globally visible elements
     const ASIS_element_id_to_ASIS_MapType& elemMap();
           ASIS_element_id_to_ASIS_MapType& elemMap_update();

     void ada_to_ROSE_translation(_Nodes_Struct& head_nodes, SgSourceFile* file);
   }


#endif /* _ADA_TO_ROSE_TRANSLATION */
