// t0007.c
// GNU dynamically sized arrays

void f(int sz)
{
  int arr[sz];
}

// not ok for globals
int blah;
//ERROR(1): int dorf[blah];
