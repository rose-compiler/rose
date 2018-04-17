// cc.in27
// template classes

// forward decl
template <class T> class Foo;

// a redudant forward decl
template <class T> class Foo;

//ERROR(1): template <class TT, class YY> class Foo;     // inconsistent

template <class T>
class Foo {
public:
  T x;

  T put();
  T get();
};

//ERROR(2): template <class TT> class Foo {};   // already defined

template <class T>
T Foo<T>::put()
{
  //return 3;
  return x;     // require Foo<T>::x to be in scope
}

//ERROR(3): template <class T> T Foo::get() { return 3; }   // needs template args

int main()
{
  Foo<int> h;
  h.x;
  //ERROR(4): h.y;   // no field
  
  Foo<int*> g;
  *( g.x );        // will complain that 'typename T' isn't a pointer..
  
}



