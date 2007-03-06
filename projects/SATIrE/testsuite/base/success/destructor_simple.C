// Simpler variant of test case destructor.C. Replaced C++ library stuff
// (string, iostream) by C constructs to get a smaller ICFG for testing.
#include <cstdio>

class A
{
public:
    A(const char *s) : varname(s)
    {
        printf("constructor for '%s' called\n", varname);
    }

    ~A()
    {
        printf("destructor for '%s' called\n", varname);
    }

private:
    A();
    const char *varname;
};

int main()
{
    A a("a");
    {
        A b("b");
    }
 // std::string foo("foo");

    return 0;
}
