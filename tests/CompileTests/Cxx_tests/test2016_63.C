// template specialization test
int* x;

class X {
  int a;
  void f() {
    a=1;
#pragma reversible map forward=original
    a=2;
  }
};

template<typename T>
void swap(T a,T b) {
  T t=a;
  b=a;
#pragma reversible map forward=original
  t=t;
}

int main() {

  swap<int>(1,2);

return 0;
}
