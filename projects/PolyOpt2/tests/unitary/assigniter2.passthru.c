
int main()
{
  int i;
  int j;
  int N;
  int A[N][N];
  
#pragma scop
{
    int c3;
    int c1;
    if (N >= 1) {
      for (c1 = 0; c1 <= N + -1; c1++) {{
          for (c3 = 0; c3 <= N + -1; c3++) {{
              A[c1][c3] = c1 * c3 / N;
            }
          }
        }
      }
    }
  }
  
#pragma endscop
}
