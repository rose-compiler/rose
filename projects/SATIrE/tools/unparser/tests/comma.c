#include "comma.h"

/* #undef n2l */
/* #define n2l(c,l)        (l =((unsigned long)(*((c)++)))<<24L, \ */
/*                          l|=((unsigned long)(*((c)++)))<<16L, \ */
/*                          l|=((unsigned long)(*((c)++)))<< 8L, \ */
/*                          l|=((unsigned long)(*((c)++)))) */


foo(iv, encrypt)
unsigned char *iv;
int encrypt;
{
  int a,b;
  register long tout0;

  if(encrypt) {
    n2l(iv,tout0);
  }
  
}
