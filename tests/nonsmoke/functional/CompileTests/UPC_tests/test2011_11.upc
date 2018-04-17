#include <upc.h>

// DQ (2/17/2011): This works fine withouth THREADS specified on the command line:
shared int a[THREADS];

// DQ (2/17/2011): This failes unless THREADS is specified on the command line:
// shared [3] int arrA[THREADS * 3];
 
int main() {
   return 0;
}
