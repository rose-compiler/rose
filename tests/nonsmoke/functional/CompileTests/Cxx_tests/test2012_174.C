
template<typename _Tp>
struct _Vector_base
   {
     struct _Vector_impl
        {
          int x;
          _Vector_impl() {}
        };

     public:
//        _Vector_base() {}

     public:
          _Vector_impl _M_impl;
   };

template<typename _Tp > class vector : protected _Vector_base<_Tp> { };


int main()
   {
     vector<int>   integerVector;
     vector<float> floatVector;

  // integerVector = vector<int >(size);

     return 0;
   }

