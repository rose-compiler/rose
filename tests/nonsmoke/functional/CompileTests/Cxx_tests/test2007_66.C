#if 0
when compiling the attached file with ROSE I get the following error:
rose_unix.c: In function _PR_UnixInit:
rose_unix.c:14: error: union __unnamed_class___F0_L3_C5_sigaction_variable_declaration__variable_type_i_variable_name_sigaction__scope__sa_handler
has no member named __sigaction_handler

The reason for that is the interesting use of macros. I didnt know
that this was the way they did
it, but it is actually part of the sigaction.h system header file so
this is a problem I do not know how we should solve. It hits the heart
of our macro handling and I can only see Wave as a solution to this.

Andreas 

#endif


struct sigaction
  {
    union
      {
	    int sa_handler;
      }
    __sigaction_handler;
    # define sa_handler	__sigaction_handler.sa_handler
};

void _PR_UnixInit(void)
{
   struct sigaction sigact;
    sigact.sa_handler;

}

