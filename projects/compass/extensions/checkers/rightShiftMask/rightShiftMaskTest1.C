#include <stdio.h>                                                              
                                                                                
int main()                                                                      
{                                                                               
  int stringify = 0x80000000;                                               
  printf("stringify: %d\n", stringify);                                         
  printf("stringify: %d\n", stringify >> 24);                                   
  printf("stringify: %d\n", (stringify >> 24) & 0xff);                            
  printf("stringify: %d\n", (stringify >>= 24) & 0xff);
  printf("stringify: %d\n", stringify);
  printf("stringify: %d\n", stringify & 0xff);
  int j = 0xff;
  j &= stringify >> 24;
  return 0;                                                                     
}     
