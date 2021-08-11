#include "test-main.h"

const char* description = "Tests default arguments for ctors";
const char* expectedout = "{AdefB~B~A}";

struct A
{
    A(const char* opt = defaultOpt)  { printf("A%s", opt); }
    ~A() { printf("~A"); }

  private:
    static const char* defaultOpt;
};


struct B : A
{
  B()  { printf("B"); }
  ~B() { printf("~B"); }
};

void run()
{
  B b;
}

const char* A::defaultOpt = "def";
