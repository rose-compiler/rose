// 13.1a.cc

class X {
    static void f();
    //ERROR(1): void f();                      // ill-formed
    //ERROR(2): void f() const;                // ill-formed
    //ERROR(3): void f() const volatile;       // ill-formed
    void g();                   
    void g() const;                // OK: no static g
    void g() const volatile;       // OK: no static g
};

