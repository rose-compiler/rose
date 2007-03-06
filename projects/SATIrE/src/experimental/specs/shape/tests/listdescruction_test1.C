#include <iostream>
#include <list>

class A {
public:
  ~A() { std::cout << "destructor A\n"; }
};

class L {
public:
  std::list<A*> l;
  ~L() { 
    for(std::list<A*>::iterator i=l.begin(); i!=l.end(); ++i) {
      delete *i;
    }
  }
};

int main() {
  L* x=new L();
  x->l.push_back(new A());
  x->l.push_back(new A());
  delete x;
  return 0;
}
