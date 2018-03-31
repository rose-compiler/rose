
template<typename _Tp>
struct allocator
   {
  // typedef _CharT char_type;
     typedef _Tp value_type;
   };

template<typename _CharT>
struct char_traits
   {
  // typedef _CharT char_type;
   };

template<>
struct char_traits<unsigned int>
   {
  // typedef unsigned int char_type;
     typedef unsigned int int_type;
   };

template<typename _Tp>
struct _Vector_base
   {
  // typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template rebind<_Tp>::other _Tp_alloc_type;
  // typedef typename __gnu_cxx::__alloc_traits<_Tp_alloc_type>::pointer pointer;
   };

// template<typename _Tp, typename _Alloc = allocator<_Tp> >
// class vector : protected _Vector_base<_Tp, _Alloc>
template<typename _Tp >
class vector : protected _Vector_base<_Tp>
   {
  // typedef typename _Alloc::value_type                _Alloc_value_type;

  // typedef _Vector_base<_Tp, _Alloc>			 _Base;
  // typedef typename _Base::_Tp_alloc_type		 _Tp_alloc_type;
  // typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type>  _Alloc_traits;

     public:
       // typedef _Tp					 value_type;
   };


vector<char_traits<unsigned int>::int_type> local2;

// Put this into a header file if required.
vector<unsigned> method1();


vector<unsigned> local1 = method1();
