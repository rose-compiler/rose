//compass_forbidden_function_vfork() is a function meant to simulate a system
//routine. This makes no difference as an example of how forbiddenFunctions
//checks errors as it works only on function names. The reason we have used
//this example is to prevent compass from treating this test file as an error
//during `make verify'

int compass_forbidden_function_vfork();
void helloWorld(int, char**);

double function();
double good_function();

namespace A {
  double function2();
}

struct B {
  void memberFunction();
};

int main(int argc, char** argv) {
  int w;
  w = compass_forbidden_function_vfork() + 5;
  helloWorld(argc, argv);
  double x = 3.0 * function();
  double y = 5.0 * good_function();
  double z = A::function2();
  B b;
  b.memberFunction();
  int (*fp)() = compass_forbidden_function_vfork;
  return 0;
}
