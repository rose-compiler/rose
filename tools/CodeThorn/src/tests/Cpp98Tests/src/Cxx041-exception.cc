#include "test-main.h"

const char* description = "Tests Basic Exception handling";
const char* expectedout = "{C0WSswcC1WSx1}";


void service(int escape)
{
  printf("S");
  if (escape) throw escape;
  printf("s");
}


void work(int escape)
{
  printf("W");
  service(escape);
  printf("w");
}

void test(int escape)
{
  try
  {
    printf("C%c", '0' + escape);
    work(escape);
    printf("c");
  }
  catch (float)
  {
    printf("f");
  }
  catch (int i)
  {
    printf("x%c", '0' + i);
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

