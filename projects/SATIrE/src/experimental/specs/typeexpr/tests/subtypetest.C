class A {};
class B : public A {};
class C : public A {};
class D : public B, C {};
class E : public B {};
class F : public E, A {};
class G : public F, D {};

void f()
{
    A a; B b; C c; D d; E e; F f; G g;
    a; b; c; d; e; f; g;
}
