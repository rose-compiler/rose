/// pair holds two objects of arbitrary type.
template<class _T1>
struct pair
   {
     typedef _T1 first_type;

     _T1 first;

     template<class _U1> pair(const pair<_U1> & __p) : first(__p.first) { }
   };

