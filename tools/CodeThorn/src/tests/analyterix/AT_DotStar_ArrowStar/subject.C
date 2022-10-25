void func(int) {

}

void func2(int) {

}

void func3(int) {

}

class A {
 public:
  int i;
  void (*fp)(int);

  A() : i(1), fp(func) {

  }
};

int main() {
  int A::*int_mem_ptr = &A::i;
  A a;
  int* ip = &((a.*int_mem_ptr) = 2);
  A* ap = new A();
  int* ip2 = &((ap->*int_mem_ptr) = 3);

  void (*(A::*void_int_func_mem_ptr))(int) = &A::fp;
  A a2;
  void (**fp)(int) = &((a2.*void_int_func_mem_ptr) = func2);
  A* ap2 = new A();
  void (**fp2)(int) = &((ap2->*void_int_func_mem_ptr) = func3);

  A a3, a4, a5;
  A* ap3 = new A();
  void (**fp3)(int) = &(&a4 > &a5 ? ((a3.*void_int_func_mem_ptr) = func2) : (ap3->*void_int_func_mem_ptr));
}
