// g0027.cc
// valid client code interacts with buggy gcc-2 headers

// these should be in namespace std, but are not
class string {};
void getline(string s);      // this prototype is bogus

// clients then expect them to be in std
void foo()
{
  std::string s;
  std::getline(s);
}

