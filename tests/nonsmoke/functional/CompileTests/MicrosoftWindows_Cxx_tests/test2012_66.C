
namespace A
   {
      template <typename T> class X
      {
        public:
             class Zone;
      };

  // template <typename T> class Zone;
   }

// template <typename T> class A::X::Zone{};

// DQ: GNU g++ requires that the specialization occur inside of namespace "A"
// Not true for EDG.  It appears that g++ ignores the namespace qualifier!
namespace A
   {
  // Required syntax for g++ 3.4.3 compiler (also works for g++ 3.3.2)
     template < typename T > class A::X<T>::Zone {};
   }

// template <typename T> class A::Zone{};

// Make this simpler (it still failes for the same reason).
// A::X<A::X<int>::Zone> globalVar;
A::X<int> globalVar;
