// t0468.cc
// 'using' declaration refers to a dependent qualified name,
// that is then used

template <class T, class U>
struct B {
  void f();
  void g();
  void h();
  void j();
};

template <class U>
struct A : public B<char, U> {
  // simplest case 'using', then a use
  using B<char, U>::f;
  void call_f() {
    f();
  }
  
  // a normal func, then 'using' brings in an overloaded version
  int g(int);
  using B<char, U>::g;
  void call_g() {
    g(3);                   // declared here
    g();                    // imported
    //ERROR(1): g(3,4);     // not declared anywhere (only found upon instantiation)
  }

  // two normal funcs, so it's already overloaded
  int h(int);
  int h(int, int);
  using B<char, U>::h;
  void call_h() {
    h(3);                   // declared here
    h(3,4);                 // also here
    h();                    // imported
    //ERROR(2): h(3,4,5);   // not declared anywhere
  }

  // 'using' comes first
  using B<char, U>::j;
  int j(int);
  void call_j() {
    j(3);                   // declared here
    j();                    // imported
    //ERROR(3): j(3,4);     // not declared anywhere
  }
};

void foo()
{
  A<int> a;
  a.call_f();
  a.call_g();
  a.call_h();
  a.call_j();
}
