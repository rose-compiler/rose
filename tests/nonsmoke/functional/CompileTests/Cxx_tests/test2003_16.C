
#if 0
ORIGINAL CODE:

void userexitfn (fn_ptr) 
  int (*fn_ptr) (); 	
{
  if (fn_ptr != NULL) user_exit_fn = fn_ptr;
}


GENERATED CODE:
extern void userexitfn(int (* fn_ptr)());
void userexitfn()
{
  if (fn_ptr != NULL) user_exit_fn = fn_ptr;
}

SHOULD HAVE GENERATED:
void userexitfn(int (* fn_ptr)()) {}
#endif


extern void userexitfn(int (*fn_ptr)(void));

int (*user_exit_fn) (void) = 0 ;

#if 0
void userexitfn (fn_ptr)
  int (*fn_ptr)();
#else
/* void userexitfn(int (*fn_ptr)(void)); */
void userexitfn(int (*fn_ptr)())
#endif
{
#if 0
  fn_ptr = 0;
#else
  if (fn_ptr != 0) user_exit_fn = fn_ptr;
#endif
}

