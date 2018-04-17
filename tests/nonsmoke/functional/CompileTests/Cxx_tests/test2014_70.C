template <typename T> struct identity { typedef T type; };

identity<int> A;
identity<int> B;

namespace X
   {
     extern identity<int> A;
   }

namespace X
   {
     identity<int> A;
   }
