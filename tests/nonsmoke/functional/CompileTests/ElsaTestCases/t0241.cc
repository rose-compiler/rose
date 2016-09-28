// t0241.cc
// specialization tries to construct a base class; the spec's
// ctor takes a specialization parameter; the passed argument
// is a member of the base class
//
// from nsAtomTable.i, nsCOMPtr<nsISupports>::nsCOMPtr( const nsCOMPtr<nsISupports>& aSmartPtr )

class Base {
public:
  int ptrField;

  Base(int p);
};


template <class T>
class Derived {            
  // this is just for looking at selfname for a primary in the debug trace
  Derived *whatever();
};

template <>
class Derived<int> : public Base {
public:          
  // use selfname with args
  Derived(Derived<int> const &obj)
    : Base(obj.ptrField)
  {}
                           
  // use selfname without args
  Derived(Derived const &obj, int foo)
    : Base(obj.ptrField)
  {}
};
