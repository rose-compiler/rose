// This test code demonstates a case where there is sharing or IR nodes.
// The _Rb_tree_impl template class is shared with the declartion of
// the class being in both the class X definition and the variable
// declaration for _M_impl.

// The later one is an error.

// The bug is only demonstrated when the template class _Rb_tree_impl
// is defined within a template class.

template <class T>
class X 
   {
     public:
          template<typename _Key_compare>
          struct _Rb_tree_impl
             {
               _Rb_tree_impl() {}
             };

     _Rb_tree_impl<int> _M_impl;
   };
