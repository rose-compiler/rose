#include <vector>

struct B
{
    int b;
};

class A
{
    int a, b, c;
    std::vector<int> v;
    B* p;

    void foo()
    {
        if (a)
            b = 0;
        if (a)
            b = 0;
        if (a)
            b = 0;
        if (a)
            b = 0;
        if (a)
            b = 0;
        if (a)
            b = 0;
        if (a)
            b = 0;
        if (a)
            b = 0;
    }
};
