// RC-65:

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

