
#define _GLIBCXX_BEGIN_NAMESPACE(X) namespace X { 
#define _GLIBCXX_END_NAMESPACE } 

_GLIBCXX_BEGIN_NAMESPACE(std)

typedef unsigned long ptrdiff_t;

template<typename _Category, typename _Tp, typename _Distance = ptrdiff_t, typename _Pointer = _Tp*, typename _Reference = _Tp&>
struct iterator
    {
      typedef _Category  iterator_category;
      typedef _Tp        value_type;
      typedef _Distance  difference_type;
      typedef _Pointer   pointer;
      typedef _Reference reference;
    };

template<typename _Iterator>
struct iterator_traits
    {
      typedef typename _Iterator::iterator_category iterator_category;
      typedef typename _Iterator::value_type        value_type;
      typedef typename _Iterator::difference_type   difference_type;
      typedef typename _Iterator::pointer           pointer;
      typedef typename _Iterator::reference         reference;
    };

template<typename _Tp>
struct iterator_traits<_Tp*>
    {
      typedef _Tp                         value_type;
      typedef ptrdiff_t                   difference_type;
      typedef _Tp*                        pointer;
      typedef _Tp&                        reference;
    };

_GLIBCXX_END_NAMESPACE

_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)

  using std::iterator_traits;
  using std::iterator;

template<typename _Iterator, typename _Container>
class __normal_iterator
    {
    protected:
      _Iterator _M_current;

    public:
      typedef typename iterator_traits<_Iterator>::value_type  value_type;
      typedef typename iterator_traits<_Iterator>::difference_type
                                                             difference_type;
      typedef typename iterator_traits<_Iterator>::reference reference;
      typedef typename iterator_traits<_Iterator>::pointer   pointer;

      __normal_iterator() : _M_current(_Iterator()) { }

      explicit
      __normal_iterator(const _Iterator& __i) : _M_current(__i) { }
    };

template<typename _Tp>
class new_allocator
   {
     public:
          typedef _Tp*       pointer;
          typedef const _Tp* const_pointer;
          typedef _Tp&       reference;
          typedef const _Tp& const_reference;
          typedef _Tp        value_type;

          template<typename _Tp1> struct rebind { typedef new_allocator<_Tp1> other; };
   };

_GLIBCXX_END_NAMESPACE

#define __glibcxx_base_allocator  __gnu_cxx::new_allocator

_GLIBCXX_BEGIN_NAMESPACE(std)

template<typename _Tp> class allocator: public __glibcxx_base_allocator<_Tp> {};

template<typename _Tp, typename _Alloc>
struct _Vector_base
   {
     typedef typename _Alloc::template rebind<_Tp>::other _Tp_alloc_type;

     struct _Vector_impl 
        : public _Tp_alloc_type
        {
          _Tp*           _M_start;
        };

     public:
          typedef _Alloc allocator_type;

     public:
          _Vector_impl _M_impl;
   };

template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
class vector : protected _Vector_base<_Tp, _Alloc>
   {
          typedef _Vector_base<_Tp, _Alloc>			 _Base;
          typedef vector<_Tp, _Alloc>			 vector_type;
          typedef typename _Base::_Tp_alloc_type		 _Tp_alloc_type;

     public:
          typedef _Tp					 value_type;
          typedef typename _Tp_alloc_type::pointer           pointer;
          typedef typename _Tp_alloc_type::const_pointer     const_pointer;

          typedef __gnu_cxx::__normal_iterator<pointer, vector_type> iterator;
          typedef __gnu_cxx::__normal_iterator<const_pointer, vector_type> const_iterator;

          const_iterator begin() const { return const_iterator(this->_M_impl._M_start); }
          iterator end();
          const_iterator end() const;

          template<typename _InputIterator> void insert(iterator __position, _InputIterator __first,_InputIterator __last);
   };

_GLIBCXX_END_NAMESPACE

class FieldDescriptor {};

class FieldGroup 
   {
     public:
          void Append(const FieldGroup& other) 
             {
            // This is unparses as: 
            //    (this) -> fields_ . insert< class __gnu_cxx::__normal_iterator< const_pointer  , vector_type  >  > ((this) -> fields_ . end(),other . fields_ . begin(),other . fields_ . end());
            // But should unparse with name qualification to be:
            //    (this) -> fields_ . insert< class __gnu_cxx::__normal_iterator< std::vector< const FieldDescriptor * , class std::allocator< const FieldDescriptor * >  >::const_pointer  , 
            //                                                                    std::vector< const FieldDescriptor * , class std::allocator< const FieldDescriptor * >  >::vector_type  >  > 
            //         ((this) -> fields_ . end(),other . fields_ . begin(),other . fields_ . end());
            // With const_pointer --> std::vector< const FieldDescriptor * , class std::allocator< const FieldDescriptor * >  >::const_pointer
            // and  vector_type   --> std::vector< const FieldDescriptor * , class std::allocator< const FieldDescriptor * >  >::vector_type
               fields_.insert(fields_.end(), other.fields_.begin(), other.fields_.end());
             }

     private:
         std::vector<const FieldDescriptor*> fields_;
   };

