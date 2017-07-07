
int main()
{
  int N;
  int M;
  int Q;
  int P;
  int a;
  int b;
  int c;
  int i;
  
#pragma scop
{
    int c1;
    if (N >= 1) {
      if (N <= M + -1 && Q <= M + -1 && Q >= P + 1) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            a;
            b;
            c;
          }
        }
      }
      if (N >= M && Q <= M + -1 && Q >= P + 1) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            b;
            c;
          }
        }
      }
      if (P <= M + -1 && Q >= M) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            b;
            c;
          }
        }
      }
      if (P >= M && Q >= P + 1) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            b;
          }
        }
      }
      if (P <= M + -1 && Q <= P) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            b;
            c;
          }
        }
      }
      if (P >= M && Q <= P) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            b;
          }
        }
      }
    }
  }
  
#pragma endscop
}
