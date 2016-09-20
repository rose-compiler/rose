void shfree(shared void *ptr);

void foo(void)
   {
     shared int *sptr;

  /* BUG: shfree(((void *)(((char *)((int *)(((char *)sptr) + MPISMOFFSET))) + MPISMOFFSET))); */
  /* BUG: shfree(((void *)(((char *)sptr) + MPISMOFFSET))); */
     shfree(sptr);
   }
