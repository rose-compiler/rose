void foo(); // not OK: used as friend in N::C
namespace N { void foo(); }
namespace N2 { void foo(); } // not OK: used as friend in N::C and ::D

namespace N {
    class C {
    public:
        friend void foo();
        friend void ::foo();
        friend void N::foo();
        friend void N2::foo();
    };
}

namespace N {
    float operator+(const C &, float);
}

#include <iostream>

class D {
public:
    friend void foo();
    friend void N2::foo();
};

namespace N {
    std::ostream &operator<<(std::ostream &, const C &);
}

using N::C;

int operator+(const C &, int); // not OK: not in N
std::ostream &operator<<(std::ostream &, const C &); // not OK: not in N

std::ostream &operator<<(std::ostream &, const D &);





void f(); // not OK: used as friend in class N::A, not in same namespace
namespace N
{
    class A
    {
    public:
        friend void ::f();
    };
}

namespace M
{
    void f(); // OK: used as friend in M::B, same namespace

    class B
    {
    public:
        friend void f();
    };
}
