// t0479.cc
// invoke method via -> in a template

struct nsISupports {
  void AddRef();
};

template <class T>
void f(nsISupports *p)
{
  p->AddRef();
}

template <class T>
void g(T *t)
{
  t->~T();
}


