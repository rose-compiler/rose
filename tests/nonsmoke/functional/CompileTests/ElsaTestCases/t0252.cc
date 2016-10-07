// t0252.cc
// forward-declare a template class specialization
// needed for ostream, char_traits<char>
        
// fwd declare the primary
template <class T>
class A;

// fwd declare a specialization
template <>
class A<int>;

// use the primary so as to cause decl instantiation
A<float> *primary;

// use the spec in a benign way
A<int> *spec;

// implement the spec
template <>
class A<int> {
public:
  int x;
};

// use spec
int f()
{
  return spec->x;
}

// define primary
template <class T>
class A {
public:
  T x;
};

// use primary, forcing instantiation
float g()
{
  return primary->x;
}


// EOF
