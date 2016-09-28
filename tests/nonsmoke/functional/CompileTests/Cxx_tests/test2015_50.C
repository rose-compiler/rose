// This class is required.
template < typename _Tp >
class new_allocator
   {
   };

#if 0
namespace XXX
   {
     template < typename _Tp >
     class allocator : public new_allocator<_Tp>
        {
        };
   }
#endif

extern template class new_allocator<int> ;
