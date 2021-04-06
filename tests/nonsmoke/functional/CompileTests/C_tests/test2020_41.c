

// void foobar (int, ...);

#include <stdarg.h> /* library of macros for "..." */

int bits(unsigned int x, char sr, ...)  
   {
#if 0
     int a, b=0;va_list listptr;   /* declare list pointer */
     va_start(listptr, sr);        /* start after var sr   */

  /* step through list: cover case of initial 0  */    
     b+=1<<va_arg(listptr,int);    
     while ((a=va_arg(listptr,int)) != 0)
         b+=1<<a;            /* use shift to get 2^a  */
     va_end(listptr);      /* clean up for return   */
     if (sr == 's') 
          return(x|b);
     if (sr == 'r')
          return(x&~b);
       else 
          return(x);
#else
     return 42;
#endif
   }
