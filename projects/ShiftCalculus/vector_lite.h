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

//        void push_back(const _Tp & t);
//        void push_back( _Tp t);
   };

template<typename _Tp > class vector : protected _Vector_base<_Tp> 
   {
     public:
          void push_back( _Tp t);
 
   };

template<typename T, typename S> class pair
   {
     public:
          pair(T t, S s);
   };


