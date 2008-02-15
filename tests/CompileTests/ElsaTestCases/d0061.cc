template<class T, class S>
T f(T x, S y) {
  f<int, int*>(3, &x);
}

template<class T, class S>
T f2(T x, S y) {
  f2(3, &x);
}

// can we have a "right" var that gets bound from the left first
// before being bound from the right
template<class T>
T g(int x, T y) {
  return g(y, y);
}

int main() {
  int q;
  f(q, &q);
  f2(q, &q);
  g(3, 4);
}
