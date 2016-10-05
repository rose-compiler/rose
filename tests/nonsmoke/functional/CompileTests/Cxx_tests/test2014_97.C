typedef struct Boundary_s 
   {
     int* edx_send[2] ;
   }  Boundary_t ;

void fillEdgeLists( int *(Boundary_s::* indices)[2] )
   {
   }

void foo()
   {
     fillEdgeLists(&Boundary_t::edx_send);
   }
