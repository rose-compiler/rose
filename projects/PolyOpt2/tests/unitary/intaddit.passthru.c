
int main()
{
  int N;
  int b[N][N];
  int a[N][N];
  
#pragma scop
{
    int c3;
    int c1;
    if (N >= 4) {
      for (c1 = 2; c1 <= N + -2; c1++) {{
          for (c3 = 2; c3 <= N + -2; c3++) {{
              b[c1][c3] = (0.2 * (a[c1][c3] + a[c1][c3 - 1] + a[c1][1 + 1]));
            }
          }
        }
      }
    }
  }
  
#pragma endscop
}
