typedef struct Boundary_s 
   {
     int    len_esend1[2];
     int    *edx_send[2] ;
   }  Boundary_t ;

void fillEdgeLists( int (Boundary_s::* len)[2], int *(Boundary_s::* indices)[2] )
   {
   }

void foo()
   {
     fillEdgeLists(&Boundary_t::len_esend1, &Boundary_t::edx_send);
   }
