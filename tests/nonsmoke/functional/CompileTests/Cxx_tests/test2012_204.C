
namespace X
   {
  // A template class.
     template <class T> class A {};

  // A template non-member function.
     template <class T> void foo_X(T x);

  // A template variable.
  // template <class T> A<T> Var;

     template <class T> struct B 
        {
          static int count;
       // static template <class S> void foo_B(S x);
          static void foo_B(T x);
        };

  // A template variable.
     template<class T> int B<T>::count = 0;

     namespace Z
        {
          int abc;
        }
   }

namespace Y
   {
     using X::A;

     using X::foo_X;

     using X::Z::abc;

  // using X::B;
  // using foo_B;

  // How do we express using a template member function?
  // using X::B<int>::foo_B<float>;
     struct Z : private X::B<int>
        {
       // using foo_B<float>;

        };
   }


