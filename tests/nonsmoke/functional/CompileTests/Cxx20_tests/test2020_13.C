
// C++17 example should difficulty: see https://en.cppreference.com/w/cpp/language/range-for

struct cow_string { /* ... */ }; // a copy-on-write string
cow_string str = /* ... */;
 
// for(auto x : str) { /* ... */ } // may cause deep copy
 
for(auto x : std::as_const(str)) { /* ... */ }

