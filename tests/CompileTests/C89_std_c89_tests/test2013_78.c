/* This is a test of static and inline keywords
   which appear to be a problem for the -std=c89 
   mode of C.
 */

static __inline__ void *
alloc_block()
{
  return 0L;
}
