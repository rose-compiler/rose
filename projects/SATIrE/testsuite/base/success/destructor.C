#include <iostream>
#include <string>

class A
{
public:
    A(std::string s) : varname(s)
    {
        std::cout << "constructor for '" << varname<< "' called" << std::endl;
    }

    ~A()
    {
        std::cout << "destructor for '" << varname<< "' called" << std::endl;
    }

private:
    A();
    std::string varname;
};

int main()
{
    A a("a");
    {
        A b("b");
    }
    std::string foo("foo");

    return 0;
}
