class _Compare {};
class _Node_allocator {};

template <class T>
class X
   {
     public:
          template<typename _Key_compare, bool _Is_pod_comparator = true>
          struct _Rb_tree_impl : public _Node_allocator
             {
               _Key_compare		_M_key_compare;

               _Rb_tree_impl() : _M_key_compare() { _M_initialize(); }

               private:
                    void _M_initialize(){}	    
             };

     _Rb_tree_impl<_Compare> _M_impl;
   };
