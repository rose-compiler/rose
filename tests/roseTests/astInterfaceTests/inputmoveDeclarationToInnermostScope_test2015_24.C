#define HAVE_SAM

#ifdef HAVE_SAM

#include <vector>

#if 0
namespace std
   {
     template<typename _Tp> class allocator {};

     template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
     class vector
        {
          public:
               vector(){}
               void push_back(_Tp x){}
        };
   }
#endif

namespace XXX {

class BndMapping 
   {
     public:
          BndMapping();
   };
}



namespace XXX {

class BlockMapping 
   {
     public:

          void reorganizeBoundaries();

          void addBoundary(const BndMapping& boundary) { m_boundaries.push_back(boundary); }

     private:
          std::vector<BndMapping> m_boundaries;
   };

}

namespace XXX 
{

}  // closing brace for namespace statement

#endif  // closing endif for HAVE_SAM guard



