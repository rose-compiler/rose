// This test code demonstrates a bug for the g++ 3.4.3 compiler
// the generated code from ROSE works fine with g++ 3.3.2.


#if 0
// Attempt to reproduce case where va_list is C++ header files is found to be
// in std namespace, but the std namespace had not been seen yet!
typedef void* va_list;

namespace std
   {
     using ::va_list;
   }
#endif

#if 0
namespace std
   {
     int x;
   }

namespace std
   {
     int y = x;
   }

namespace X
   {
     int x;
     int y = x;
   }
#endif

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

#ifdef __GNUC_PREREQ
#if (__GNUC_PREREQ(3,4) == 0)
  // Works for g++ 3.3.2 compiler but not g++ 3.4.3
     class A::X<int>::Zone{};
#endif
#endif
   }

// template <typename T> class A::Zone{};

A::X<A::X<int>::Zone> globalVar;
