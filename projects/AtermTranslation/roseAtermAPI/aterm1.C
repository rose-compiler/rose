#include "rose.h"

#include "rose_aterm_api.h"


// void rose_ATinit(int argc, char *argv[], ATerm *bottomOfStack)
void rose_ATinit(int argc, char *argv[], void* bottomOfStack)
   {
  // Initial implementation.
     ROSE_ASSERT(!"rose_ATinit(): not implemented!");
   }

// rose_ATerm rose_ATreadFromTextFile(FILE *file)
ATerm rose_ATreadFromTextFile(FILE *file)
   {
  // Initial implementation.

  // We don't have to support reading the AST from a file since it is already in memory.
     printf ("ERROR: We don't have to support reading the AST from a file since it is already in memory. \n");
     ROSE_ASSERT(false);

     return NULL;
   }
   

// rose_ATbool rose_ATmatch(ATerm t, const char *pattern, ...)
bool rose_ATmatch(ATerm t, const char *pattern, ...)
   {
  // Initial implementation.
     ROSE_ASSERT(!"rose_ATmatch(): not implemented!");
     return false;
   }


