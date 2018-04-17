// clash between function and instantiated template parameter member

// originally found in package netkit-telnet

// error: during argument-dependent lookup of `foo', found non-function of
// type `int' in class B at 0x82258b8

// ERR-MATCH: during argument-dependent

class B { int foo; };
template <class T> class A { };
typedef A<B> C;

int foo(C *);

void bar() {
    C* c;
    foo(c);
}
