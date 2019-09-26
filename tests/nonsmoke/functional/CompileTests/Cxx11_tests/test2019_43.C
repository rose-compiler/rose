template<class T>
struct A
   {
     struct X
        {
          typedef int I;
	};
   };

template<class T>
struct B : public A<T>
   {
     using typename A<T>::X;
     static int f() 
        {
          typename X::I c;
          return sizeof(c);
        }
   };

void foobar()
   {
     int a = B<char>::f();
   }
