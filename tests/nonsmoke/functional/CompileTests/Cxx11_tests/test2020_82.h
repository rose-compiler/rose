// ROSE-2410 (recursive macro)

namespace Namespace_1 {
  class Class_1 {
    public:
    enum Enum_1 {
      Enum_Val_1
    };
  };
}

// REQUIRED FOR ERROR: define name and enum name are the same:
#define Enum_Val_1 Namespace_1::Class_1::Enum_Val_1

