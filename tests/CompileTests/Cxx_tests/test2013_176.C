// This test code tries to reproduce the problem with the map container 

// Templated class
template <typename T, typename U = int>
class Xt
   {
     public:
          Xt ();

       // template <typename V> Xt ( V v, U u );
   };

#if 1
// Example of code that should be generated for the specialization of Xt template using default parameter
template <>
class Xt<int,int>
   {
     public:
       // Added typedef to allow for unconverted default parameters appearing in template declarations!
       // typedef int U;

       // The bug is that this is not output in the generated code.
          Xt ();

       // Xt (int x);

       // template <typename V> Xt ( V v, U u );
          template <typename V> Xt ( V v );
   };
#endif

// foo() forces use of Yt<int> which forces use of Xt<T>
void foo()
   {
     Xt<int> x;
  // Xt<int,int> y;
   }



