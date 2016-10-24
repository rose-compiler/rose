// t0293.cc
// cast from templatized derived to template param base
// but no other syntax to instantiate the Derived class

class Base {
public:
};

template <class T>
class Derived : public T {
};

//class Derived2 : public Base {
//};


void find(Base* instance);
void find(const char * mimetype);

void foo()
{
  Derived<Base> *d;
  //Derived<Base> inst;
  //Derived2 *d2;

  find(d);     // overload resolution; need to cast to Base
  //find(d2);
}
