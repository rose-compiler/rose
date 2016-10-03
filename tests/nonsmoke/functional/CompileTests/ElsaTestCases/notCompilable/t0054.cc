// t0054.cc
// explicit template class specialization

// primary template
template <class T>
class A {
  //T f();    // not declared!
};

// explicit specialization
template <>
class A<char> {
  // something new in the specialization
  char f() { return 'f'; }
};

int main()
{
  A<char> a;

  // now that specialization matching is implemented, this call to 'f'
  // will be looked up in the proper specialization
  a.f();

  A<int> b;
  // ERROR(1): b.f();     // doesn't work since base doesn't have 'f'

  return 0;
}
