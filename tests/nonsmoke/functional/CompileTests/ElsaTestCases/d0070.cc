// minimized from nsAtomTable.i

struct A {};
template<class T> struct B {};

template<class T> void f(B<T> &) {}
template<class T> void f(const B<T> &) {}

int g(B<A> &b) {
  f(b);
}
  
//  int main() {
//    B<A> b;
//    f(b);
//  }
