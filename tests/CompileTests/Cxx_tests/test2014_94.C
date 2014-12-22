
typedef struct Boundary_s 
   {
   }  Boundary_t ;


void fillEdgeLists(int *(Boundary_s::* indices)[2] )
   {
     Boundary_t boundary;
     unsigned dir = 0;

     int const *cur_indices = (boundary.*indices)[dir];
   }
