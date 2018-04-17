// This fails the new test for unique entries in the type cache (new EDG/ROSE translation).

template<typename, typename>
struct __are_same
   {
  // enum { __value = 0 };
  // typedef __false_type __type;
   };

template<typename _Tp>
struct __are_same<_Tp, _Tp>
   {
  // enum { __value = 1 };
  // typedef __true_type __type;
   };

