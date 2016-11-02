// test recursive function template

template<class T> T f(T x) {
  // well, an infinite loop, but so what
  return f<int>(x);
}

template<class T> T g(T x) {
  // well, an infinite loop, but so what
  return g(x);
}

int main() {
  f<int>(3);
  g(3);
}
