// cc.in36
// testing template implemenation some more
  
template <class T>
class Baz {
public:
  int b(T *p);
};


int func()
{
  Baz<int> b;
  return (int)3;
}



template <class T>
class Foo {
  T *x;
  Baz<T> *b;

  T *f() {
    int q;
    q = b->b(x);
    return x;
  }
};

int func2()
{
  Foo<float> f;
  return *(f.x);    // should be type 'float'
}

