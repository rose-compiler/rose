// cc.in56
// using "typename" to dig down into a template argument to
// retrieve a type

class A {
public:
  typedef int diff_type;
};

template <class T>
class Foo {
public:
  // class T is expected to contain a nested (or typedef'd) type
  // called 'diff_type', and here we're digging down to retrieve
  // it, and give it a nicer alias in this scope
  typedef typename T::diff_type diff_type;

  // use the retrieved type
  diff_type d;
};

// example usage
int main()
{
  Foo<A> f;
  int x = f.d;     // 'f.d' has type 'int'
  return x;
}
