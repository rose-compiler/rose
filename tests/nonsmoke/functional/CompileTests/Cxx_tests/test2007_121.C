// #include<vector>

#if 1
// typedef int integer;
   typedef int integer;

class XYZ
   {
     public:
          template<typename T1, typename S = integer > struct iterator;

          template<typename T2, typename S >
          struct iterator
             {
                typedef T2 iterator_category;
                typedef S difference_type;
             };
   };


XYZ::iterator<int> i;
#endif
