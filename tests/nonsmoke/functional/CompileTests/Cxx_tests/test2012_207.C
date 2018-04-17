// This is a isue in ROSE, where the non-defining declarations built for each specialization reference the 
// defining declaration of the original template (I think) and not their own defining declaration.

// struct __true_type { };
// struct __false_type { };

template<typename _Tp>
struct __is_integer
   {
  // enum { __value = 0 };
  // typedef __false_type __type;
   };

template<>
struct __is_integer<bool>
   {
  // enum { __value = 1 };
  // typedef __true_type __type;
   };

#if 1
template<>
struct __is_integer<char>
   {
  // enum { __value = 1 };
  // typedef __true_type __type;
   };
#endif
