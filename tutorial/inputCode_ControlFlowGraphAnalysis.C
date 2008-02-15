#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[])
{
  int i;
  char buffer[10];
  for (i=0; i < strlen(argv[1]); i++)
  {
 // Buffer overflow for strings of over 9 characters
    assert(i < 10);
    buffer[i] = argv[1][i];
  }
  return 0;
}

#if 0
void 
bar(int& w)
   {
     ++w;
   }

int 
main(int, char**)
   {
     int z = 3;
     int a = 5 + z + 9;
     int b = (6 - z) * (a + 2) - 3;
     bar(b);
     while (b - 7 > 0)
        {
          b-=5; 
          --b;
        }

     do {
          --b; LLL: if (b <= -999) return 0;
        }
     while (b > 2);

     for (b = 0; b < 10; ++b)
          ++z;

     for (int z2 = 7 + z * 5; z2 + 9 < b % 10; ++*(&z2 + 5 - 5))
        {
          (a += 7) += 7; 
          ++(++a);
        }
     b = -999;
     goto LLL;
   }
#endif

