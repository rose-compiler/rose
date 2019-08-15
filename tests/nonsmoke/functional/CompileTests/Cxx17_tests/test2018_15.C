// New Rules for auto deduction from braced-init-list

// This is required.
#include <initializer_list>

auto x1 = { 1, 2 }; // decltype(x1) is std::initializer_list<int>
// auto x2 = { 1, 2.0 }; // error: cannot deduce element type
// auto x3{ 1, 2 }; // error: not a single element
auto x4 = { 3 }; // decltype(x4) is std::initializer_list<int>
auto x5{ 3 }; // decltype(x5) is int
