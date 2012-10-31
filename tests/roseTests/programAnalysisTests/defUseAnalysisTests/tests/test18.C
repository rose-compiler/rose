
int foo()
{
  int i=0;
  int* p;
 for (i=0;i<10;i++)
 {
   int x = i;
   *p = x;
   *p = i;
   int z = *p;
   //++p;
   int e;
   p = &i;
 }
 return 0;
} 
