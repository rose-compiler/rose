class X
   {
     public:
          X();
   };

template <class _Alloc>
class _List_base 
   {
     public:
          typedef _Alloc allocator_type;
          allocator_type get_allocator() const { return allocator_type(); }
   };

_List_base<X> Y;
