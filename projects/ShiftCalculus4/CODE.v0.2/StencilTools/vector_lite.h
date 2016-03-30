
#ifndef _VECTOR_LITE_H_
#define _VECTOR_LITE_H_

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

template<typename T, int S> class array
   {
     public:
          array(T t, int s);
   };

namespace std {
template<typename T, typename S, typename R = S> class tuple
   {
     public:
     tuple(T t, S s, R r);
   };

template <class T1,class T2> pair<T1,T2> make_pair (T1 x, T2 y);

}


double abs ( double d );

#endif


