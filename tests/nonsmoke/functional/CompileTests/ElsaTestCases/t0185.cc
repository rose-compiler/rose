// t0185.cc
// explicit specialization of template class constructor
// cppstd 14.7.3 para 4

template <class T>
struct A {
public:
  A(const char *s);
};
                    
// what if A<char> has already been instantiated?
//typedef A<char> foo;

// what if I leave out the "template <>"?  parse error...
template <>
A<char>::A(const char*);
