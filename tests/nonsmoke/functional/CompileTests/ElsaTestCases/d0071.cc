template <class T>
void m(const T & a, const T & b) {};
void g() {
  const int x = 3;
  m(x, 4);
}
