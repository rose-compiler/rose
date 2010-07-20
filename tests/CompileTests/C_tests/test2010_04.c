/* Example failing code reported 7/18/2010): by King
   This appears to be a legitimate problem with the 
   code and not ROSE. gcc allows this, but should
   have problems if it were linked, were as ROSE 
   (actually EDG) catches it at compile time.
   The problem is that "hash" is a function defined in 
   file scope, so it has no external linkage outside of
   the file.  But the inline function ("test") could
   be used by another file (if it were defined in a
   header file) and then the program could not link.
   EDG is detecting this problem, though for this single
   source file it would not seem to be a problem.
   The C Specification 6.7.4.2 states:
   "An inline definition of a function with external linkage shall not contain a definition
    of a modifiable object with static storage duration, and share not contain a reference 
    to an identifier with internal linkage."
   The problem here is that "hash" is the name of an internal linkage function
   within "test" which is an external linage inlined function.
   Thus EDG catches the problem, and GNU gcc does not.
 */

static unsigned int
hash (str, len)
register char *str;
register unsigned int len;
{
}
inline int
test()
{
        register char* str;
        register int len;
        hash(str, len);
}
