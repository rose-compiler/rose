#include <iostream>
 
struct Empty {}; // empty class
 
struct X {
    int i;
    Empty e;
};
 
struct Y {
    int i;
    [[no_unique_address]] Empty e;
};
 
struct Z {
    char c;
    [[no_unique_address]] Empty e1, e2;
};
 
struct W {
    char c[2];
    [[no_unique_address]] Empty e1, e2;
};
 
int main()
{
    // the size of any object of empty class type is at least 1
    static_assert(sizeof(Empty) >= 1);
 
    // at least one more byte is needed to give e a unique address
    static_assert(sizeof(X) >= sizeof(int) + 1);
 
    // empty member optimized out
    std::cout << "sizeof(Y) == sizeof(int) is " << std::boolalpha 
              << (sizeof(Y) == sizeof(int)) << '\n';
 
    // e1 and e2 cannot share the same address because they have the
    // same type, even though they are marked with [[no_unique_address]]. 
    // However, either may share address with c.
    static_assert(sizeof(Z) >= 2);
 
    // e1 and e2 cannot have the same address, but one of them can share with
    // c[0] and the other with c[1]
    std::cout << "sizeof(W) == 2 is " << (sizeof(W) == 2) << '\n';
}


