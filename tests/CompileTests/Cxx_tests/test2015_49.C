// This class is required.
template < typename _Tp >
class new_allocator
   {
   };

namespace XXX
   {
     template < typename _Tp >
     class allocator : public new_allocator<_Tp>
        {
        };
   }

// This is an error for GNU g++, but works fine for ROSE (EDG).
// However, the generated ROSE code fails for GNU, because the
// original code fails for GNU.
// template<> class XXX::allocator<int> ;
extern template class XXX::allocator<int> ;

