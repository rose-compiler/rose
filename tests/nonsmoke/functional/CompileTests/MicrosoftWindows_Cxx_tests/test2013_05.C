
template<typename _Tp>
struct _Vector_base
   {
     struct _Vector_impl
        {
          _Vector_impl() {}
        };

     public:
          _Vector_impl _M_impl;
   };

_Vector_base<int>   integerVector;



