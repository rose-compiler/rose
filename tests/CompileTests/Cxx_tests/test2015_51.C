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

extern template class XXX::allocator<int> ;
