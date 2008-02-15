/*
The following code compiled with ROSE:

extern void *memcpy (void *__restrict __dest,
      __const void *__restrict __src, unsigned int __n)
    throw () __attribute__ ((__nonnull__ (1, 2)));


gives the following error:

lt-correctExternalComments:
/home/andreas/REPOSITORY-SRC/ROSE/June-29a-Unsafe/NEW_ROSE/src/backend/unparser/name_qualification_support.C:32:
bool Unparser::isAnOuterScope(SgScopeStatement*, SgScopeStatement*):
Assertion `targetScope != __null' failed. 
*/

extern void *memcpy (void *__restrict __dest,
      __const void *__restrict __src, unsigned int __n)
    throw () __attribute__ ((__nonnull__ (1, 2)));

