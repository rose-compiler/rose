/* This is a bug in the unparser:
   The function is unparsed as: static char *memory(register size)
   where the use of "register" is an error.
 */
static char *memory(size)
register unsigned int size;
   {
     register char *p = 0; /* (char *) calloc(size, 1); */
     return (p);
   }
