//#include <sys/time.h>

void func() {}

int main() {
  // gettimeofday is implicitly declared in C89: There is no declaration
  //  to get the parameter types from: Assume implicit function address-taking
  gettimeofday(func, 0);
}
