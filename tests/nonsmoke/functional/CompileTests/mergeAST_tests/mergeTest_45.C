// This examples demonstrates issues specific to member function function constructors
  template<typename _Tp>
    class new_allocator
    {
    public:
#if 0
      typedef int        size_type;
   // typedef ptrdiff_t  difference_type;
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;
      typedef _Tp        value_type;
#endif
#if 0
      template<typename _Tp1>
        struct rebind
        { typedef new_allocator<_Tp1> other; };
#endif
      new_allocator() throw() { }

//    new_allocator(const new_allocator&) throw() { }
#if 0
      template<typename _Tp1>
        new_allocator(const new_allocator<_Tp1>&) throw() { }
#endif
      ~new_allocator() throw() { }
#if 0
      pointer
      address(reference __x) const { return &__x; }

      const_pointer
      address(const_reference __x) const { return &__x; }

      // NB: __n is permitted to be 0.  The C++ standard says nothing
      // about what the return value is when __n == 0.
      pointer
      allocate(size_type __n, const void* = 0)
      { return static_cast<_Tp*>(::operator new(__n * sizeof(_Tp))); }

      // __p is not permitted to be a null pointer.
      void
      deallocate(pointer __p, size_type)
      { ::operator delete(__p); }

      size_type
      max_size() const throw() 
      { return size_t(-1) / sizeof(_Tp); }

      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 402. wrong new expression in [some_] allocator::construct
      void 
      construct(pointer __p, const _Tp& __val) 
      { ::new(__p) _Tp(__val); }

      void 
      destroy(pointer __p) { __p->~_Tp(); }
#endif
    };
#if 0
  template<typename _Tp>
    inline bool
    operator==(const new_allocator<_Tp>&, const new_allocator<_Tp>&)
    { return true; }
  
  template<typename _Tp>
    inline bool
    operator!=(const new_allocator<_Tp>&, const new_allocator<_Tp>&)
    { return false; }
#endif

new_allocator<char> X;
