// This is the essential part of the bug in test2012_105.c (zftp.c from the zsh C application)

// If SUPPORT_IPV6 is defined to be 0 then this code WILL compile.
// If SUPPORT_IPV6 is defined to be 1 then this code FAILS to compile.

#define SUPPORT_IPV6 1

void foobar()
   {
     int x;
#if SUPPORT_IPV6
     for(;;)
# define MACRO() break
#else
# define MACRO() 43;
#endif
        {
          MACRO();
        }
   }
