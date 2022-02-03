#ifndef _ADA_TO_ROSE_TRANSLATION
#define _ADA_TO_ROSE_TRANSLATION 1

#include "a_nodes.h"

namespace Ada_ROSE_Translation
   {
     const int UNTYPED_FILE_ID = 0;
     const int MAX_NUMBER_OF_UNITS = 10;

     using ASIS_element_id_to_ASIS_MapType = std::map<int,Element_Struct*> ;

     void ada_to_ROSE_translation(Nodes_Struct& head_nodes, SgSourceFile* file);

     extern ASIS_element_id_to_ASIS_MapType asisMap;
   }


#endif /* _ADA_TO_ROSE_TRANSLATION */
