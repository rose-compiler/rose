// This is a similar case to test2003_27.C

template < class T1 >
class reverse_iterator
   {
  // #define XYZ ABC
     T1 abc;
   };

#if 0
template <class T1, class T2 = int, class T3 = long >
class XYZ
   {
     public:
          typedef T1* pointer;
          typedef pointer iterator;
          typedef ::reverse_iterator<iterator> reverse_iterator;
   };
#endif
