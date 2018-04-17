void foo();
namespace A
   {
     void foo();
     void foobar()
        {
          foo();
          ::foo();
        }
   }
