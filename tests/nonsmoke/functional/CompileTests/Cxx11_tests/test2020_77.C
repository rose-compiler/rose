// ROSE-2577

namespace Namespace_1 {
  namespace Shared_Name_1 {
    class Shared_Name_1 {
    };
    class Class_2 {
    // Required for error:
      public: Class_2();
    };
    class Class_1 : public Class_2 {
      public: Class_1();
    };
  }
}
using namespace Namespace_1::Shared_Name_1;
Class_1::Class_1() : Class_2() {
}
// Unparses OK:
Class_2 global_1 = Class_2();



// gets this error:
// rose_ROSE-88.cpp(27): error: class "Namespace_1::Shared_Name_1::Shared_Name_1" has no member class "Class_2"
//   Class_1::Class_1() : Shared_Name_1::Class_2()
//                                       ^

