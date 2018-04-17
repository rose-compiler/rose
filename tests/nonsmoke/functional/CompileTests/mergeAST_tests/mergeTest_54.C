// This demonstrates a problem with making the initalizers significant in the sharing process
// (forcing then to kill any opportunity to share).  The code to make the initializer cause a 
// unique name is in the SageInterface (for case SgInitializedName and SgVariableDeclaration).
static inline int
__gthread_active_p (void)
{
// static void *const __gthread_active_ptr = (void *) &__gthrw_pthread_create;
  static void *const __gthread_active_ptr = (void *) 0L;
  return __gthread_active_ptr != 0;
}

