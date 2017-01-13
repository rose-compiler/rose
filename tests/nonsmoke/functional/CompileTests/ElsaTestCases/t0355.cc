// t0355.cc
// testing that msvc/m0001.cc is rejected in non-MSVC mode

class Integer {
public:
  // this is fine
  int operator<(int);
  
  // this is illegal, but allowed by MSVC
  //ERROR(1): operator==(const Integer&) const { return 0; }
};
