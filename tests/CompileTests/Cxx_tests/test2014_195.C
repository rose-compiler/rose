template <typename T>
class X
   {
     public:
#if 1
       // Note that template and non-template class declarations are not normalized to be forward declarations.
          template <typename S>
          class A
             {
               public:
                    S x;
#if 1
                    A();
#else
                    A() 
                       { 
                         int a_value;
                       }
#endif
             };
#endif

#if 0
         X();
#else
         X() 
             {
               int x_value;
             }
#endif
   };

#if 0
// If we treat this as name qualification then it has to be seperated into two parts (to leave space for the return type).
// It might be better to NOT treat this as name qualification and form the qualified name directly with the template 
// function name (and parameter list).  Template names are then treated differenty than non-template function names.
template < typename T >
X<T>::X () { int x_value; }
#endif

#if 1
// If we treat this as name qualification then it has to be seperated into two parts (to leave space for the return type).
// It might be better to NOT treat this as name qualification and form the qualified name directly with the template 
// function name (and parameter list).  Template names are then treated differenty than non-template function names.
template < typename T >
template < typename S >
X<T>::A<S>::A () { int a_value; }
#endif

void foo()
   {
      X<int> a;
      X<int>::A<int> b;
   }

