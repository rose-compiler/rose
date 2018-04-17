#include "boost/pool/pool.hpp"

namespace 
   {
     template<std::size_t Alignment>
     struct AlignedAllocator 
        {
          typedef std::size_t size_type;
          typedef std::ptrdiff_t difference_type;

#if 0
          static void free(char * ptr);
#else
          static void free(char * ptr) {}
#endif
        };

#if 0
template < std :: size_t Alignment >
void AlignedAllocator<Alignment>::free ( char * ptr ) {}
#endif

     typedef boost::pool<AlignedAllocator<32> > Pool;

     Pool pool(0,8);

   } // unnamed namespace
