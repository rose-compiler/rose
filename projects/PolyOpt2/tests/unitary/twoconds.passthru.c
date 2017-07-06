
int main()
{
  int M;
  int N;
  int a;
  int b;
  int i;
  
#pragma scop
{
    int c1;
    if (N >= 1) {
      if (M <= N) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            b;
          }
        }
      }
      if (M >= N + 1) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            a;
          }
        }
      }
    }
  }
  
#pragma endscop
}
