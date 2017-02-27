#include <cstddef>

void f(int *intp)
{
    // Passed an int pointer
}

void f(char *charp)
{
    // Passed a char pointer
}

// DQ (2/21/2017): This should be "std::nullptr_t" instead of "nullptr_t"
// void f(std::nullptr_t nullp)
// void f(nullptr_t nullp)
void f(std::nullptr_t nullp)
{
    // Passed a null pointer
}

// int* pointer = 
