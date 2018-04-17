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
            // DQ (8/31/2014): These two statements have unparse the default template argument and failed to name qualify it.
            // Note that this is unparsed as: typedef boost::function< double (const ::std_xxx::vector < Extents_t , allocator< Extents_t > > &) > CostFunction;
               typedef boost::function<double (std_xxx::vector<Extents_t> const &)> CostFunction;
            // Note that this is unparsed as: boost::function< double (const ::std_xxx::vector < Extents_t , allocator< Extents_t > > &) > CostFunction_var;
               boost::function<double (std_xxx::vector<Extents_t> const &)> CostFunction_var;
        };

} // namespace XXX

