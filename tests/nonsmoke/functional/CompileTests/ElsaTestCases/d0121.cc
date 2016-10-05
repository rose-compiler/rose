template < class charT > struct basic_string {
  basic_string (const charT * s) {}
};

typedef basic_string <char> string;

struct Geometry {
  //ERROR(1): Geometry (std::string geometry_);
  void f();
};

void Geometry::f() {
  const char *geometry;
  
  // this line used to cause a segfault; now it properly
  // causes an error message
  //ERROR(2): *this = std::string (geometry);
}
