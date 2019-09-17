#include <cstddef>
#include <sstream>
#include <iostream>

size_t identity(size_t n)
{
  return n;
}

size_t sum(size_t n)
{
  return n*(n+1) / 2;
}

size_t fib(size_t n, size_t fib1, size_t fib2)
{
  if (n == 0) return fib1+fib2;

  return fib(n-1, fib1+fib2, fib1);
}

size_t fib(size_t n)
{
  if (n < 2) return n;

  return fib(n-2, 1, 0);
}

size_t execute(const size_t mode, size_t count)
{
  static const size_t NUM_FUNCS = 3;

  size_t op  = 0;
  size_t res = 0;

  while (count)
  {
    switch (op)
    {
      default:
      case 0:
        res += identity(count);
        break;

      case 1:
        res += sum(count);
        break;

      case 2:
        res += fib(count);
        break;
    }

    if (mode == 1)
      op = (op+1) % NUM_FUNCS;

    --count;
  }

  return res;
}

void helpMsg()
{
  std::cout << "test-execution-monitor [mode [num]]\n"
               "   mode ..   0 execute only identity function\n"
               "             1 alternate identity, sum, and fib functions\n"
               "\n"
               "   num  .. when mode is 0, or 1 num specifies the numnber of iterations\n"
               "           the default is 0\n";
}

template <class T, class U>
T cast(const U& u)
{
  std::stringstream conv;
  T                 t;

  conv << u;
  conv >> t;
  return t;
}

int main(int argc, char** argv)
{
  size_t mode  = 0;
  size_t count = 10;
  
  if (std::string(argv[1]) == "-h")
  {
    helpMsg();
    return 0;
  }

  if (argc > 1) mode  = cast<size_t>(argv[1]);
  if (argc > 2) count = cast<size_t>(argv[2]);

  if (mode < 2) 
    execute(mode, count);
  else 
    helpMsg();
  
  return 0;
}
