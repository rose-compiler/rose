// This test code tries to reproduce the problem with the map container 

// Templated class
template <typename T, typename U = int>
class Xt
   {
     public:
          int x;
          typedef int Integer;

          Xt ();

          Xt (T t, U u);

          template <typename V>
          Xt ( V v, U u );
   };

#if 1
// Example of code that should be generated for the specialization of Xt template using default parameter
template <>
class Xt<int,int>
   {
     public:
          int x;
          typedef int Integer;

       // Added typedef to allow for unconverted default parameters appearing in template declarations!
          typedef int U;

          Xt ();

          Xt (int t, int u);

          template <typename V>
          Xt ( V v, U u );
   };
#endif

// main() forces use of Yt<int> which forces use of Xt<T>
int
main()
   {
     Xt<int> x;
     Xt<int,int> y;
     return 0;
   }



