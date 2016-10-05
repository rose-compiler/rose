
class Internal_Index
   {
   };

void foo ( const Internal_Index & I )
   {
     Internal_Index* Index_Pointer_List; // = 0L;

  // This is unparsed as: "Index_Pointer_List = &I;"
     Index_Pointer_List = &((Internal_Index &) I);
   }

