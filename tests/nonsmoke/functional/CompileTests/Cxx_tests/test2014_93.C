
typedef struct Boundary_s 
   {
   }  Boundary_t ;


void fillEdgeLists(int (Boundary_s::* len)[2])
   {
     Boundary_t boundary;
     unsigned dir = 0;

     int cur_len = (boundary.*len)[dir];
   }
