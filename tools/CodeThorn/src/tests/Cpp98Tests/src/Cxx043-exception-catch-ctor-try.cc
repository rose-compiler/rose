#include "test-main.h"

const char* description = "Compares various subobjects of a class";
const char* expectedout = "{TASsWt~W~S~ATAS~Axi1xi1}";

struct A
{
  A() { printf("A"); }
  ~A() { printf("~A"); }
};


struct Service
{
  Service(int escape)
  {
    printf("S");
    if (escape) throw escape;
    printf("s");
  }

  ~Service() { printf("~S"); }
};

struct Work : A, Service
{
  Work(int escape)
  try
  : A(), Service(escape)
  {
    printf("W");
  }
  catch (int i)
  {
    printf("xi%c", '0'+i);
  }

  ~Work() { printf("~W"); }
};

void test(int escape)
{
  try
  {
    printf("T");
    Work w(escape);
    printf("t");
  }
  catch (int i)
  {
    printf("xi%c", '0'+i);
  }
  catch (...)
  {
    printf("?");
  }
}

void run()
{
  test(0);
  test(1);
}

