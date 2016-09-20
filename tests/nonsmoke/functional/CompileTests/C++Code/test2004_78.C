
typedef int globalIntegerType;

namespace A
   {
     namespace B
        {
          void foo_B();
          int nonClassMemberData;

          class baseClass
             {
               public:
                   int publicBaseClassMemberData;
             };

           class Bclass : baseClass
             {
               public:
                    int publicClassMemberData;
               protected:
                    int protectedClassMemberData;
               private:
                    int privateClassMemberData;

               void memberFunctionFoo();

            // This is a using declaration (member using-declaration: type)
               using baseClass::publicBaseClassMemberData;
             };
        }

  // This is a using declaration (non-member using-declaration: routine)
     using B::foo_B;

  // This is a using declaration (non-member using-declaration: variable)
     using B::nonClassMemberData;

  // This is a using declaration (non-member using-declaration: type)
     using B::Bclass;
   }


namespace A
   {
     using namespace B;

  // using publicClassMemberData;

     void foobar1() 
        {
          foo_B();
        } 
   }

// A more complex usage:

namespace A
   {
     void foo();
     void foo_A();

     namespace B
        {
          namespace C
             {
               void foo_C();
             }
          using namespace C;
        }
   }

namespace A
   {
     namespace B
        {
          using namespace A;
          using namespace C;
          void foobar2()
             {
               foo();
               foo_A();
               A::foo();
               B::foo();
               foo_C();
             }
        }
   }


int main()
   {
     using namespace A;
//   using namespace A::B;
     foobar1();
     foobar2();

     return 0;
  }
