
typedef struct 
   {
     int size;
   } *htab_t;


/* The parens around the function names in the next two definitions
   are essential in order to prevent macro expansions of the name.
   The bodies, however, are expanded as expected, so they are not
   recursive definitions.  */

#define rose_macro_htab_size(htab)  ((htab)->size)

// size_t (htab_size) (htab_t htab)
// int foo_htab_size (htab_t htab)
int (htab_size) (htab_t htab)
{
  return rose_macro_htab_size (htab);
}

