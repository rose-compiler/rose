template <typename T1, typename T2>
class pair
   {
   };

typedef unsigned long ptrdiff_t;

template<typename _Tp>
pair<_Tp*, ptrdiff_t> foo()
    {
      return pair<_Tp*, ptrdiff_t>();
    }

template<typename _Tp>
pair<_Tp*, ptrdiff_t> foobar()
{ return pair<_Tp*, ptrdiff_t>(); }
