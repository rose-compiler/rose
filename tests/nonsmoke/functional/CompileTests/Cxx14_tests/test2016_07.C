
#include <string>

string  lookup1();
string& lookup2();

// C++11 code
string  look_up_a_string_1() { return lookup1(); }
string& look_up_a_string_2() { return lookup2(); }

// C++14 code (see test2016_08.C)
// decltype(auto) look_up_a_string_1() { return lookup1(); }
// decltype(auto) look_up_a_string_2() { return lookup2(); }


