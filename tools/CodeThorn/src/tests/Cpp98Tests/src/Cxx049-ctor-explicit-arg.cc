#include "test-main.h"

const char* description = "Tests default arguments for ctors";
const char* expectedout = "{AexpB~B~A}";

struct A
{
    A(const char* opt = defaultOpt) : val(opt) { printf("A%s", val); }
    ~A() { printf("~A"); }

  private:
    const char* val;

  protected:
    static const char* defaultOpt;
    static const char* explicitOpt;

};


struct B : A
{
  B() : A(A::explicitOpt) { printf("B"); }
  ~B() { printf("~B"); }
};

void run()
{
  B b;
}

const char* A::defaultOpt = "def";
const char* A::explicitOpt = "exp";
