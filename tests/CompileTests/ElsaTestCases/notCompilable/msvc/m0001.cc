// m0001.cc
// report from Kevin Millikin about MSVC bug

class Integer {
public:
  // this is fine
  int operator<(int);
  
  // this is illegal, but allowed by MSVC
  operator==(const Integer&) const { return 0; }
};
