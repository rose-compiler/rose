namespace std
{
  typedef long unsigned int 	size_t;
// typedef long int	ptrdiff_t;
// typedef decltype(nullptr)	nullptr_t;
}


namespace std
{
  
  template<class _E>
    class initializer_list
    {
    public:
#if 0
      typedef _E 		value_type;
      typedef const _E& 	reference;
      typedef const _E& 	const_reference;
      typedef const _E* 	iterator;
      typedef size_t 		size_type;
#endif
      typedef const _E* 	const_iterator;

    private:
#if 0
      iterator			_M_array;
      size_type			_M_len;
#endif
      
   // constexpr initializer_list(const_iterator __a, size_type __l);
   // constexpr initializer_list(const_iterator __a, size_type __l) : _M_array(__a), _M_len(__l) { }
      constexpr initializer_list(const_iterator __a, size_t __l);

    public:
   // constexpr initializer_list() noexcept : _M_array(0), _M_len(0) { }
   // constexpr initializer_list() noexcept;

#if 0
      constexpr size_type size() const noexcept;
   // constexpr size_type size() const noexcept { return _M_len; }

      
      constexpr const_iterator begin() const noexcept;
   // constexpr const_iterator begin() const noexcept { return _M_array; }

      constexpr const_iterator end() const noexcept;
   // constexpr const_iterator end() const noexcept { return begin() + size(); }
#endif
    };

#if 0
   template<class _Tp> constexpr const _Tp* begin(initializer_list<_Tp> __ils) noexcept;
// template<class _Tp> constexpr const _Tp* begin(initializer_list<_Tp> __ils) noexcept { return __ils.begin(); }

   template<class _Tp> constexpr const _Tp* end(initializer_list<_Tp> __ils) noexcept;
// template<class _Tp> constexpr const _Tp* end(initializer_list<_Tp> __ils) noexcept { return __ils.end(); }
#endif
}

class X
   {
     public:
          X (std::initializer_list<int> list);
   };

X x = {42};

