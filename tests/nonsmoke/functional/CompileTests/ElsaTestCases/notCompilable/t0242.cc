// t0242.cc
// overload resolution involving complete specialization
// from nsAtomTable.i, nsGetterAddRefs<nsISupports>::~nsGetterAddRefs()

class A;


// primary
template <class T>
class B {
};

// specialization for A
template <>
class B<A> {
};


// two overloaded functions that do stuff with 'B'
template <class T>
void func(B<T> &x);

template <class T>
void func(B<T> const &x);


void g()
{
  B<A> &y;
  func(y);     // should call first 'func'
}
