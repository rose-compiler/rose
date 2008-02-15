
#include <stdio.h>
#include <new>

using namespace std;

// const nothrow_t std::nothrow;

int main(int argc, char **argv) {
   int l = 10;

// Original problem line of code
// int *i = new (nothrow) int[l];

   int *i = new (nothrow) int[l];

   printf("&i is %p\n", i);
   return 0;
}


