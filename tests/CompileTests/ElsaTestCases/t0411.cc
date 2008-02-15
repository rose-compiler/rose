// t0411.cc
// variant of t0410.cc


class string {
  string (char const *src);
};

template < class T >
class StringSObjDict {
  T *remove (char const *key, int);    // genuinely not viable
  T *remove (string const &key)
  {
    char const *s;
    return remove (s);                 // recursive call
  }
};
                       
class A {};

void foo()
{
  StringSObjDict<A> s;
}


// EOF
