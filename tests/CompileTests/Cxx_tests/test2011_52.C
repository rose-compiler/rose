namespace ns_B
   {
     class C{};
   }

namespace ns_C
   {
     template <typename T> class B{};
     typedef B<ns_B::C> B_class_B_C;
   }
