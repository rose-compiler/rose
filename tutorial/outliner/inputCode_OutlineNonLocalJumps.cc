#include <iostream>

size_t factorial (size_t n)
{
  size_t i = 1;
  size_t r = 1;
  while (1)
    {
#pragma rose_outline
      if (i <= 1)
	break; // Non-local jump #1
      else if (i >= n)
	break; // Non-local jump #2
      else
	r *= ++i;
    }
  return r;
}

int main (int argc, char* argv[])
{
  std::cout << "7! == " << factorial (7) << std::endl; // Prints 5040
  return 0;
}
