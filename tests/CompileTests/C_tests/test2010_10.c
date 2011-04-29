/*
Liao 11/9/2010
A simplified test case from spec_cpu2006's 403.gcc
*/
int
vasprintf (result, format, args)
     char *result;
     char *format;
#if defined (_BSD_VA_LIST_)
     _BSD_VA_LIST_ args;
#else
     char** args;
#endif
{
  return 0;
}

int
vasprintf2 (
     char *result,
     char *format,
#if defined (_BSD_VA_LIST_)
     _BSD_VA_LIST_ args,
#else
     char** args
#endif
     )
{
  return 0;
}

