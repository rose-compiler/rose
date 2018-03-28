typedef struct
   {
     char *name;
     int pass;
   } testcall;


// This is an array, and the output of the class name is not required.
static testcall alltests[] =
   {
     {"start", 24},
     {"end", 42}
   };

struct struct1
   { 
     int array1[2]; 
   };

struct struct2
   { 
     struct1 array1[2]; 
   };

struct2 local2 = {struct1 {1, 1} , struct1 {1, 1}};
