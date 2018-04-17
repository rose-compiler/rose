namespace Y
   {
  // typedef int Integer;
     typedef struct type_A typedefType_A;
//   typedef struct type_B typedefType_B;
//   struct type_B *Bptr;
   }

namespace X
   {
#if 1
     template < typename T > class A;
#else
     typedef int Integer;
     template < typename T = int > class A;
     template < typename T = A < Integer > > class B;
     template < typename T = A < X::Integer > > class C;
     template < typename T = A < Y::Integer > > class D;
#endif

//   template < typename T = B <> > class E;
     template < typename T = A < Y::typedefType_A > > class F;
//   template < typename T = A < Y::type_A > > class G;
//   template < typename T = A < Y::type_B > > class H;
   }

