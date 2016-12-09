// Extended constexpr:

// C++11 example:
constexpr int my_charcmp( char c1, char c2 ) 
   {
     return (c1 == c2) ? 0 : ((c1 < c2) ? -1 : 1);
   }
