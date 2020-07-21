#include<typeinfo>

void f(int, const int (&)[2] = {}) {} // #1
void f(const int&, const int (&)[1]) {} // #2
void test()
{
    const int x = 17;
    auto g0 = [](auto a) { f(x); }; // ok: calls #1, does not capture x
    auto g1 = [=](auto a) { f(x); }; // does not capture x in C++14, captures x in C++17
                                     // the capture can be optimized away
    auto g2 = [=](auto a) {
            int selector[sizeof(a) == 1 ? 1 : 2] = {};
            f(x, selector); // ok: is a dependent expression, so captures x
    };
    auto g3 = [=](auto a) {
      typeid(a + x);  // captures x regardless of whether a + x is an unevaluated operand
    };
}


