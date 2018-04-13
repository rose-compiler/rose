
void foo()
   {
     int a,b,c;

     a = b + c;

     int array[10];

     array[a++] = array[b] + array[c];

     a++;

     int* ptr;

     ptr[2] = 42;
     *(ptr+2) = 42;

struct X
{
  int member;
};

      X* xxx;
      xxx->member = a + b;
   }
