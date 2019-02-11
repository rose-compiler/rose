typedef struct Boundary_s 
   {
   } Boundary_t;

// This is the case in a function paremeter list (parenthesis are required).
void fillEdgeLists(int (Boundary_s::* len)[2] )
// void fillEdgeLists(int Boundary_s::* len[2] )
   {
     Boundary_t boundary;
     unsigned dir = 0;

  // This is a variable declaration with type.
     int (Boundary_s::* array)[2];

  // int const *cur_indices = (boundary.*indices)[dir];
     int cur_len = (boundary.*len)[dir];
   }
