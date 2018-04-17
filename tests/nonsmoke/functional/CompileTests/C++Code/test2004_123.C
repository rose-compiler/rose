// This example demonstrates where a namespace and a class can conflict!
namespace ns_B
   {
     class C{};
   }

namespace ns_C
   {
     template <typename T> class B{};
     class D{};

     typedef B<class ns_B::C> B_class_B_C;
     typedef B<ns_C::D> B_class_C_D;
   }

