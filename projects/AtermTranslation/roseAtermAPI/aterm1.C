#include "rose.h"

#include "rose_aterm_api.h"


// void rose_ATinit(int argc, char *argv[], ATerm *bottomOfStack)
void rose_ATinit(int argc, char *argv[], void* bottomOfStack)
   {
  // Initial implementation.

     printf ("Inside of forwarded function: rose_ATinit(int argc, char *argv[], void* bottomOfStack) \n");

  // ROSE_ASSERT(!"rose_ATinit(): not implemented!");
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


// **********************************************************************************
// These are the implementation that are not taking advantage of function forwarding.
// These are required to support the SDF support, 27 functions to support...
// **********************************************************************************


ATerm ATreadFromNamedFile(const char *name)
   {
     ROSE_ASSERT(!"ROSE ATerm API: ATreadFromNamedFile(): not implemented!");
     return NULL;
   }

ATbool ATwriteToNamedTextFile(ATerm t, const char *name)
   {
  // Initial implementation.

     ROSE_ASSERT(!"ROSE ATerm API: ATwriteToNamedTextFile(): not implemented!");
  // error: cannot convert 'bool' to 'ATbool' in return
  // return false;
   }

void ATinit(int argc, char *argv[], ATerm *bottomOfStack)
   {
  // We don't have to initialize anything that I can think of at the moment.
  // If we are implementing the API using function forwarding and the original 
  // ATerm library as a basis for our ROSE ATerm library then maybe we need to 
  // call the original ATerm library ATinit() function.

  // ROSE_ASSERT(!"ROSE ATerm API: ATinit(): not implemented!");
   }

ATerm ATreadFromString(const char *string)
   {
     ROSE_ASSERT(!"ROSE ATerm API: ATreadFromString(): not implemented!");
     return NULL;
   }

void ATprotect(ATerm *atp)
   {
   }

void ATerror(const char *format, ...)
   {
   }

void ATabort(const char *format, ...)
   {
   }

void ATprotectArray(ATerm *start, int size)
   {
   }

ATerm ATmake(const char *pattern, ...)
   {
     ROSE_ASSERT(!"ROSE ATerm API: ATmake(): not implemented!");
     return NULL;
   }

unsigned char *ATwriteToBinaryString(ATerm t, int *len)
   {
   }

ATerm ATreadFromBinaryString(const unsigned char *s, int size)
   {
     return NULL;
   }

ATbool ATwriteToNamedBinaryFile(ATerm t, const char *name)
   {
   }

void ATwarning(const char *format, ...)
   {
   }

void ATunprotect(ATerm *atp)
   {
   }

int ATfprintf(FILE *stream, const char *format, ...)
   {
     return 0;
   }

ATbool ATmatchTerm(ATerm t, ATerm pat, ...)
   {
   }

ATbool ATmatch(ATerm t, const char *pattern, ...)
   {
   }

ATerm  ATsetAnnotation(ATerm t, ATerm label, ATerm anno)
   {
     return NULL;
   }

ATerm  ATgetAnnotation(ATerm t, ATerm label)
   {
     return NULL;
   }

ATerm  ATremoveAnnotation(ATerm t, ATerm label)
   {
     return NULL;
   }

char* ATwriteToString(ATerm t)
   {
     return NULL;
   }

ATbool ATwriteToTextFile(ATerm t, FILE *file)
   {
   }

ATbool ATwriteToBinaryFile(ATerm t, FILE *file)
   {
   }

void ATunprotectArray(ATerm *start)
   {
   }

ATerm ATreadFromFile(FILE *file)
   {
     return NULL;
   }

ATbool ATisEqualModuloAnnotations(ATerm t1, ATerm t2)
   {
   }

int ATvfprintf(FILE *stream, const char *format, va_list args)
   {
     return 0;
   }



// ************* Stratego Specific API Requirements ************
// Adding 2 more functions from the Aterm API.
// *************************************************************

char *ATwriteToSharedString(ATerm t, int *len)
   {
     return NULL;
   }

long ATwriteToSharedTextFile(ATerm t, FILE *f)
   {
     return 0;
   }


















