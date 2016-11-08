#include <string>

string  lookup1();
string& lookup2();

decltype(auto) look_up_a_string_1() { return lookup1(); }
decltype(auto) look_up_a_string_2() { return lookup2(); }

