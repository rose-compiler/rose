// #include <vector>

namespace std_xxx
   {
     template<typename _Tp> class allocator {};

     template<typename _Tp, typename _Alloc = std_xxx::allocator<_Tp> >
  // template<typename _Tp>
     class vector
        {
          public:
               vector();

               class const_iterator
                  {
                    public:
                      // const_iterator & operator+ (const_iterator & X);
                      // const_iterator & operator- (const_iterator & X);
                     friend const_iterator operator+ (const_iterator X, const_iterator Y);
                     friend const_iterator operator- (const_iterator X, const_iterator Y);
                  // friend const_iterator & operator- (_Tp X, const_iterator & Y);
                     _Tp & operator* ();
                  };

               const_iterator begin();
            // friend _Tp operator* (const_iterator X);
        };
   }

typedef struct Extents_s {} Extents_t ;

#include "boost/function.hpp"

namespace XXX 
   {

     class UnstructuredBlockPartitioner 
        {
          public:
               typedef boost::function<double (std_xxx::vector<Extents_t> const &)> CostFunction;

               boost::function<double (std_xxx::vector<Extents_t> const &)> CostFunction_var;
        };

} // namespace XXX

