class A {
public:
  typedef long size_type;
  //  int f() {
  //  return __testoff?__off :(this) -> size() - __pos;
  //}
  size_type
  _M_limit(size_type __pos, size_type __off) const noexcept {
    const bool __testoff = __off < this->size() - __pos;
    return __testoff ? __off : this->size() - __pos;
  }
  int __off;
  int __pos;
  int __testoff;
  int size() const { return 20; }
};
  
int main() {
  A a;
  a.__testoff=1;
  a.__off=2;
  a.__pos=3;
  //a.f();
  return 0;
}
