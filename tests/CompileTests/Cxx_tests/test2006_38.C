
namespace X
   {
     class A
        {
          public:
               class B
                  {
                    public:
                         typedef int A;
                         void foo();
                         void foo(int);
                         void foo(int,A);
                  };
        };
   }

void
X::A::B::foo()
   {
   }

void
X::A::B::foo(int)
   {
   }

void
X::A::B::foo(int,int)
   {
   }


// void foo (int* ptr) {}

void foo (int* __restrict__ ptr) {}
