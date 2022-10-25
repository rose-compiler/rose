#include "test-main.h"

const char* description = "Compares various subobjects of a class";
const char* expectedout = "{Ex1ExATWSswtTWSExAExAx~ExA~Ex}";


struct Ex
{
  Ex()
  : data("Ex")
  {
    printf("Ex0");
  }

  Ex(const char* desc)
  : data(desc)
  {
    printf("Ex1");
  }


  ~Ex() { printf("~Ex"); }

  const char* data;
};

struct ExA : Ex
{
  ExA()
  : Ex("ExA")
  {
    printf("ExA");
  }

  ~ExA() { printf("~ExA"); }
};


void service(const ExA* ex)
{
  printf("S");
  if (ex) throw ex;
  printf("s");
}


void work(const ExA* ex)
{
  try
  {
    printf("W");
    service(ex);
    printf("w");
  }
  catch (const ExA* ex)
  {
    printf("ExA%s", ex->data);
    throw 0;
  }
  catch (const Ex* ex)
  {
    printf("Ex%s", ex->data);
  }
  catch (...)
  {
    printf("?");
  }
}

void test(const ExA* escape)
{
  try
  {
    printf("T");
    work(escape);
    printf("t");
  }
  catch (const ExA* ex)
  {
    printf("ExA%s", ex->data);
  }
  catch (const Ex* ex)
  {
    printf("Ex%s", ex->data);
  }
  catch (...)
  {
    printf("x");
  }

}

void run()
{
  ExA exa;

  test(NULLPTR);
  test(&exa);
}

