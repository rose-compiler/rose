#include <cstdarg>

char *concatenate(char const *s, ...)
{
  return 0;    
}

int main()
{
  char *separator = "\t";

  char *t = concatenate("hello", separator, "world", 0);

  return 0;
}
