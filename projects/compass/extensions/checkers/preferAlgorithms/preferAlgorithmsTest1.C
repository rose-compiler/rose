#include <vector>
#include <algorithm>
#include <functional>

void add_x_to_each_element(int x, std::vector<int> &v)
{
    // not OK: loop to add x to each element
    std::vector<int>::iterator v_itr;
    for (v_itr = v.begin(); v_itr != v.end(); ++v_itr)
        *v_itr += x;

    // OK: using an algorithm to add x to each element
    transform(v.begin(), v.end(), v.begin(),
              std::bind2nd(std::plus<int>(), x));
}

void f(int *begin, int *end)
{
    // OK: loops on integer types are still allowed
    for (int i = 0; i < 100; i++)
        ;

    // not OK: pointer loops are like iterator loops
    for (int *p = begin; p < end; p++)
        ;
}
