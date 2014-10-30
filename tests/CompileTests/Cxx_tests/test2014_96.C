typedef struct Boundary_s 
   {
     int len_esend1[2];
   }  Boundary_t ;

void fillEdgeLists( int (Boundary_s::* len)[2] )
   {
   }

void foo()
   {
     fillEdgeLists(&Boundary_t::len_esend1);
   }
