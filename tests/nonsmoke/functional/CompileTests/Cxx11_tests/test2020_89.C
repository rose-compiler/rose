// ROSE-2577 (simplified)

namespace Shared_Name_1 
   {
     class Shared_Name_1 
        {
        };
     class Class_2 
        {
       // Required for error:
          public: Class_2();
        };
     class Class_1 : public Class_2 
        {
          public: Class_1();
        };
   }

// using namespace Shared_Name_1;

// Original code:  Class_1::Class_1() : Class_2()
// Generated code: Class_1::Class_1() : Shared_Name_1::Class_2()

// Class_1::Class_1() : Class_2() 
Shared_Name_1::Class_1::Class_1() : Class_2() 
   {
   }

