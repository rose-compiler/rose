// t0410.cc
// some overloading+template problem


class string {
  string (char const *src);
};

template < class T > 
class StringSObjDict {
  void remove (char const *key);        // line 11
  void remove (string const &key)       // line 12
  {
    char const *s;
    __testOverload(remove (s), 11);
  }
};

//  class A {};

//  void foo()
//  {
//    StringSObjDict<A> s;
//  }


// EOF
