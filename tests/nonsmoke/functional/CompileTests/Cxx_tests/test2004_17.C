// Demonstration of typedef islands

#if 1
typedef class tag_class
   {
     int x;
   } ABC1;

// ABC1 x;
#endif

#if 0
typedef union tag_union 
   {
     int x;
     int y;
   } ABC2;

// ABC2 y;
#endif

#if 0
typedef struct tag_struct 
   {
     int x;
     int y;
   } ABC3;

// ABC3 z;
#endif
