#include <stddef.h> /* size_t */
void* mymemcpy(void *dest, const void *src, size_t n)
{
  char *dp = (char*) dest;
  const char *sp = (const char*) src;
  while (n--)
    *dp++ = *sp++; // (*dp=*sp, sp+=1, dp+=1)
  return dest;
}

#define N 10

int main() {
  int a[N];
  int b[N];
  for(int i=0; i<N; ++i) {
    a[i]=i+1;
    b[i]=i+2;
  }
  mymemcpy(b,a+1,N-2);
  mymemcpy(a,b+2,N-2);
  return 0;
}
