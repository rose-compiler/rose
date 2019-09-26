namespace A
   {
      template <typename T> class X
      {
        public:
             class Zone;
      };
   }

namespace A
   {
     template < typename T > class A::X<T>::Zone {};
   }

A::X<A::X<int>::Zone> globalVar;
