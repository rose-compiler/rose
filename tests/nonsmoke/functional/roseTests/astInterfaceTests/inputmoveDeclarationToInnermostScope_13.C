#include <stdio.h>
 
extern void Initialize_Iterator(int *) ;
extern bool      Check_Iterator(int *) ;
extern void    Advance_Iterator(int *) ;
 
void foo(int dimensions)
{
   int itr ;
   for (Initialize_Iterator(&itr) ;
        Check_Iterator(&itr) ;
        Advance_Iterator(&itr))
   {
      printf("hi\n") ;
   }
}


#if 0
void foo2(int dimensions)
{
   for (int itr, Initialize_Iterator(&itr) ; // illegal code here
        Check_Iterator(&itr) ;
        Advance_Iterator(&itr))
   {
      printf("hi\n") ;
   }
}
#endif 
