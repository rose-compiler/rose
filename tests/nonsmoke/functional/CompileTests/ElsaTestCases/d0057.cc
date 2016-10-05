// test forwarded function template

// forward
template<class T> T g(T x);

template<class T> T f(T x) {
  // well, an infinite loop, but so what
  return g(x);
}

template<class T> T g(T x) {
  // well, an infinite loop, but so what
  return f(x);
}

int main() {
  f<int>(3);
}
