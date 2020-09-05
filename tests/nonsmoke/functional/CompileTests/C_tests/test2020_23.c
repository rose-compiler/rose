// RC-65:

// We might be able to fix this by detecting names of constructs that match
// those in the macros defined, and then just remove the associated #define.

struct htab 
   {
     int size;
   };

typedef struct htab * htab_t;

#define htab_size(htab)  ((htab)->size)

int (htab_size) (htab_t htab) 
   {
     return htab_size(htab);
   }

