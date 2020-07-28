// Error in EDG 6.0:
// "/data1/ROSE_CompileTree/git-LINUX-64bit-6.1.0-EDG60-BOOST_1_60-dq-development-
//          rc-cxx-only/include-staging/g++_HEADERS/hdrs5/bits/stl_bvector.h",
//          line 958: internal error: assertion failed at: "overload.cc", line
//          11162 in cast_pointer_for_field_selection
//
//          && __position == end())
//                           ^


#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
 
int main()
{
    std::vector<int> c = {1, 2, 3, 4, 5, 6, 7};
    int x = 5;
    c.erase(std::remove_if(c.begin(), c.end(), [x](int n) { return n < x; }), c.end());
 
    std::cout << "c: ";
    std::for_each(c.begin(), c.end(), [](int i){ std::cout << i << ' '; });
    std::cout << '\n';
 
    // the type of a closure cannot be named, but can be inferred with auto
    // since C++14, lambda could own default arguments
    auto func1 = [](int i = 6) { return i + 4; };
    std::cout << "func1: " << func1() << '\n';
 
    // like all callable objects, closures can be captured in std::function
    // (this may incur unnecessary overhead)
    std::function<int(int)> func2 = [](int i) { return i + 4; };
    std::cout << "func2: " << func2(6) << '\n';
}


