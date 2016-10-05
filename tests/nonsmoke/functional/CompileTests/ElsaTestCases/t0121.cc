// t0121.cc
// based on 13.1a.cc

class X {
  static void f();
  //ERROR(1): void f();                      // ill-formed
  //ERROR(2): void f() const;                // ill-formed
  //ERROR(3): void f() const volatile;       // ill-formed
  void f(int);

  void g();
  void g() const;                // OK: no static g
  void g() const volatile;       // OK: no static g
  //ERROR(4): void g() const volatile;       // duplicate definition
  //ERROR(5): void g() const;                // duplicate definition
  //ERROR(6): void g();                      // duplicate definition
};


void X::f() {              // static
  //ERROR(10): this;
}
void X::f(int) { this; }   // nonstatic

void X::g() { this; }
void X::g() const { this; }
void X::g() const volatile { this; }

//ERROR(7): void X::g() {}                   // redefinition
//ERROR(8): void X::g() const {}             // redefinition
//ERROR(9): void X::g() const volatile {}    // redefinition
