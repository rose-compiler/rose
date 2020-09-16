template<class ... Types> void f(Types ... values);
void g() {
  f<int*, float*>(0, 0, 0); // Types = {int*, float*, int}
}

