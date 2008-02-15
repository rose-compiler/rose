
#if 0
int x;
int z;

template <class T>
class A 
{
 public:
      A () {};
      T foo () { T t = 0; return t; };
};

class B
{
 public:
      B () {};
};

typedef void (B::*MyPointerToMemberFunctionType)();
#endif

void
foo1 ()
   {
     int x = 0;
#if 1
     int y = 0;
     int z;
     z = x + y;
     if (z > 0)
          x = 1;
       else
          y = 2;
#endif
   }

void
foo2 ()
   {
     int x = 0;
#if 0
     int y = 0;
     int z;
     z = x + y;
#endif
   }


#if 0
main ()
   {
  // A<double> myclass;

     int a = 0;
#if 0
     int b = 1;
     int c = 2;
     int d = 3;
   
     a = b + c + d;
#endif
     return 0;
   }
#endif
