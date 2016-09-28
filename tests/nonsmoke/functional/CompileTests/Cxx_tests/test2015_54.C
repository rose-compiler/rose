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

template < typename _Tp, typename _Alloc >
class AAA
   {
     public:
//        typedef typename _Alloc::template rebind<_Tp>::other;

          struct _List_impl // : public _Node_alloc_type
             {
            // _List_node_base _M_node;
               int _M_node;

               _List_impl() : _M_node()
                 { }

            // _List_impl(const _Node_alloc_type& __a) : _Node_alloc_type(__a), _M_node() { }
             };

          _List_impl _M_impl;

          void foo();
       // void foo() { this->_M_impl._M_node; }
   };

template<> void AAA< int, XXX::allocator<int> >::foo()
   {
  // This will instantate to be (which is the error):
  // (this) -> _M_impl . _List_impl< > ::_M_node;
     this->_M_impl._M_node;
   }

// This is an error for GNU g++, but works fine for ROSE (EDG).
// However, the generated ROSE code fails for GNU, because the
// original code fails for GNU.
// template<> class XXX::allocator<int> ;
extern template class XXX::allocator<int> ;

extern template class AAA< int, XXX::allocator<int> >;


void foobar()
   {
     AAA< int, XXX::allocator<int> > a;
     a.foo();
   }

