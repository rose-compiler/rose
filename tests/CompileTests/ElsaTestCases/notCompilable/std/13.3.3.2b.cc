// 13.3.3.2b.cc

int f(const int &);                // line 3
int f(int &);                      // line 4
int g(const int &);                // line 5
int g(int);                        // line 6

int i;
int j = __testOverload(f(i), 4);   // Calls f(int &)
//ERROR(1): int k = g(i);                      // ambiguous

class X {
public:
    void f() const;                // line 14
    void f();                      // line 15
};
void g(const X& a, X b)
{
    // TODO: these aren't checked because I'm not currently
    // doing overload resolution on member function calls
    a.f();                         // Calls X::f() const
    b.f();                         // Calls X::f()
}
