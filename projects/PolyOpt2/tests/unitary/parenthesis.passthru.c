
int main()
{
  int i;
  int N;
  int A[N];
  int B[N];
  
#pragma scop
{
    int c1;
    if (N >= 1) {
      for (c1 = 0; c1 <= N + -1; c1++) {{
          A[c1] = B[2 * c1 + 1];
        }
      }
    }
  }
  
#pragma endscop
}
