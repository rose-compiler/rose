template <class T>
void foo ( int y, T u )
   {
     y = y + u;
   }

// extern "C" int _Z3fooIiEviT_ = 0;

int main()
   {
     foo(1,2);

#if 1
     foo<int>(1,2);

     foo(1,3.14);

     long longX;
     foo(1,longX);

     float floatX;
     foo<float>(1,3.14);
#endif

     return 0;
   }
