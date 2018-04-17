// t0395.cc
// ambiguous overload?

struct A
{
  bool operator!=(const char*) const;
  operator char*() const;
};

void g() {
  A a;
  if (a != 0) {}
}
