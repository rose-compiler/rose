
namespace A
   {
     template <typename T> class B{};
     class C{};
   }

namespace B
   {
     template <typename T> class B{};

  // This unparses to: "class B::C" (fixed 11/6/2004)
     class C{};

  // This unparses to: "class B::B<int>" (fixed 11/6/2004)
     typedef B<int> B_int;
   }

