// This is from (subset) test2012_08.C
// Example from C++11 Wiki web page:

#include<initializer_list>
#include<vector>
#include<string>

// std::vector<std::string> v1 = { "xyzzy", "plugh", "abracadabra" };
// std::vector<std::string> v2 ({ "xyzzy", "plugh", "abracadabra" });
// std::vector<std::string> v3 { "xyzzy", "plugh", "abracadabra" }; // see "Uniform initialization" below

std::vector<std::string> v2 ({"xyz"});
