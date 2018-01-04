#include <math.h>

int main()
{
  int i, M, N, b, c;
  int a[N];
#pragma scop
  if (M > N)
    for (i = max(2, N); i < M; i = i + 1)
      {
	a[i] = pow(b,c);
	c = c + 1;
      }
  if (M == N)
    a[0] = c;
#pragma endscop
}
