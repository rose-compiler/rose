#include<string>
#include<set>

class CaseInsensitiveString {
  std::string s;
public:
  std::weak_ordering operator<=>(const CaseInsensitiveString& b) const {
    return case_insensitive_compare(s.c_str(), b.s.c_str());
  }
  std::weak_ordering operator<=>(const char* b) const {
    return case_insensitive_compare(s.c_str(), b);
  }
  // ... non-comparison functions ...
};
 
// DQ (7/21/2020): Moved function calls into a function.
void foobar1()
   {
  // Compiler generates all four relational operators
     CaseInsensitiveString cis1, cis2;
     std::set<CaseInsensitiveString> s; // ok
     s.insert(/*...*/); // ok
     if (cis1 <= cis2) { /*...*/ } // ok, performs one comparison operation
 
  // Compiler also generates all eight heterogeneous relational operators
     if (cis1 <= "xyzzy") { /*...*/ } // ok, performs one comparison operation
     if ("xyzzy" >= cis1) { /*...*/ } // ok, identical semantics

   }

