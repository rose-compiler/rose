// t0266.cc
// basic_string::_S_copy_chars

  template <class S, class T>
  class __normal_iterator {};

  template<typename _CharT, typename _Traits, typename _Alloc>
    class basic_string
    {

    public:
      typedef typename _Alloc::pointer pointer;
      typedef typename _Alloc::const_pointer const_pointer;
      typedef __normal_iterator<pointer, basic_string> iterator;
      typedef __normal_iterator<const_pointer, basic_string>
                                                            const_iterator;

      template<class _Iterator>
        static void
        _S_copy_chars(_CharT* __p, _Iterator __k1, _Iterator __k2)
        {
        }

      static void
      _S_copy_chars(_CharT* __p, iterator __k1, iterator __k2)
      { }

      static void
      _S_copy_chars(_CharT* __p, const_iterator __k1, const_iterator __k2)
      { }

      static void
      _S_copy_chars(_CharT* __p, _CharT* __k1, _CharT* __k2)
      { }

      static void
      _S_copy_chars(_CharT* __p, const _CharT* __k1, const _CharT* __k2)
      { }

  };
