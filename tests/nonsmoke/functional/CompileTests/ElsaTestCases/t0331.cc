// t0331.cc
// use operator on dependent type in template code

template <class T>
class Ptr {
public:
  T operator* ();
};

enum E { e1 } e;

template <class T>
T deref(Ptr<T> &p)
{
  return *p << e;     // *p has dependent type when tchecking the template body
}

int f()
{
  Ptr<int> p;
  return deref(p);
}

