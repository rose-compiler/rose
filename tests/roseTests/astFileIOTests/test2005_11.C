

template<typename T>
class X
   {
     private:
     public:
          typedef int privateType;

     public:
       // template<typename T1, typename T2 = X::privateType>
       // template<typename T2>
          void foo();
   };

template<typename T1>
void X<T1>::foo()
   {
   }

class Y
   {
     private:
     public:
          typedef int privateType;

     public:
          template<typename T2> void foo() {}
          template<typename T2, typename T3> void foo() {}
   };

#if 0
template<typename T2>
void Y::foo()
   {
   }
#endif

template<typename T1>
class Z
   {
     private:
     public:
          typedef int privateType;

     public:
       // template<typename T2>
          void foo() {}
   };

#if 0
template<typename T1, typename T2>
void Z<T1>::foo(T2 t)
   {
   }
#endif

template<typename T1>
class A
   {
     private:
     public:
          typedef X<T1> privateType;

     public:
          typedef privateType publicType;
          template<typename T2>
          void foo(publicType & p) {}
   };


int main()
   {
     X<int> x;
     x.foo();

     Y y;
     y.foo<int>();
     y.foo<int,float>();

     Z<int> z;
     z.foo();
  // z.foo<Z<int> >();

     A<int> a;
     A<int>::publicType apt;
     a.foo<int>(apt);

     return 0;
   }

