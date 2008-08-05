#include <vector>

bool f(const std::vector<int> &v, size_t n)
{
    if (v.size() > 0) // not OK: use !v.empty() instead
        return true;
    if (0 == v.size()) // not OK: use v.empty() instead
        return false;
    return false;
}

bool f2(const std::vector<int> &v, size_t n)
{
    if (v.size() > n) // OK: comparing against variable
        return true;
    if (v.size() == 2) // OK: comparing against non-zero constant
        return true;
    return false;
}
