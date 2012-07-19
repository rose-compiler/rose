// recursive reference to the tag of an autonomous type in a typedef
typedef struct Atag3
   {
     Atag3* a;
   } A3;

// Declaration of variables using both the tag of an autonomous 
// type and the typedef name of the same autonomous type
Atag3 B;
// A3 C;

