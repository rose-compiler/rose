template < class T1 >
class reverse_iterator
   {
//   T1 abc;
   };

template <class T1, class T2 = int, class T3 = long >
class XYZ
   {
     public:
          typedef T1* pointer;
          typedef pointer iterator;
//        typedef int iterator;

       // I think that this shows that we must support a template instantiation inside of a template declaration,
       // and that we can't just have template declarations inside of a template declaration.  The essential 
       // difficulty was that template class declarations and class declarations were not unified as symbols,
       // so fixing that would be a better approach.
          typedef ::reverse_iterator<iterator> local_reverse_iterator;
   };
