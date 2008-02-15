/* this seems to generate the warning within ROSE of:

Error: Found a template declaration in scope = SgGlobal being marked as compiler generated!
Inside of Sg_File_Info::display(template declaration being marked as compiler generated) 
     isTransformation    = false 
     isCompilerGenerated = false 
     filename = /home/dquinlan2/ROSE/LINUX-3.3.2/g++_HEADERS/hdrs1/bits/type_traits.h 
     line     = 264  column = 1 
*/

struct __true_type {};
struct __false_type {};

template <class _Tp> struct _Is_integer {
  typedef __false_type _Integral;
};

template<> struct _Is_integer<bool> {
  typedef __true_type _Integral;
};



#if 0
template<> struct _Is_integer<char> {
  typedef __true_type _Integral;
};

template<> struct _Is_integer<signed char> {
  typedef __true_type _Integral;
};

template<> struct _Is_integer<unsigned char> {
  typedef __true_type _Integral;
};

template<> struct _Is_integer<wchar_t> {
  typedef __true_type _Integral;
};

template<> struct _Is_integer<short> {
  typedef __true_type _Integral;
};

template<> struct _Is_integer<unsigned short> {
  typedef __true_type _Integral;
};

template<> struct _Is_integer<int> {
  typedef __true_type _Integral;
};

#endif

