// function template instantiation with a default argument that is an
// E_funCall to another function template instantiation

template<class T> 
T g(T x) {
  return x;
}

template<class T2> 
T2 f(T2 y = g(3)) {
  return y;
}

int main() {
  int z = f<int>();
  z = f(4);
  return 0;
}
