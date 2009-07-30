/* header for dlopen() etc */
#include  <dlfcn.h>

/* handle to the shared lib file */
void *FunctionLib;		

/* a prototype of the pointer to a loaded routine */
void (*OUT__1__6755__) (void **__out_argv);
// ..........
int
hypre_SMGResidual( void               *residual_vdata,
                   hypre_StructMatrix *A,
                   hypre_StructVector *x,
                   hypre_StructVector *b,
                   hypre_StructVector *r)
{
  // ........
  /*  Pointer to error string   */
  const char *dlError;		

  FunctionLib = dlopen ("OUT__1__6755__.so",RTLD_LAZY);
  dlError = dlerror ();
  if (dlError)
  {
    printf ("cannot open .so file!\n");
    exit (1);
  }

  /* Find the first loaded function */
  OUT__1__6755__ = dlsym (FunctionLib, "OUT__1__6755__");
  dlError = dlerror ();
  if (dlError)
  {
    printf ("cannot find OUT__1__6755__() !\n");
    exit (1);
  }

  /* Call the outlined function by the found function pointer */
  // parameter wrapping statements are omitted here
  (*OUT__1__6755__) (__out_argv1__1527__);
  // ...
}
