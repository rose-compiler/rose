
int main()
{
  int M;
  int N;
  int K;
  int C[N][N];
  int _C_[N][N];
  int A[N][N];
  int B[N][N];
  
#pragma scop
{
    int c5;
    int c1;
    int c3;
    if (K >= 1 && M >= 1 && N >= 1) {
      for (c1 = 0; c1 <= M + -1; c1++) {{
          for (c3 = 0; c3 <= N + -1; c3++) {{
              for (c5 = 0; c5 <= K + -1; c5++) {{
                  C[c1][c3] = _C_[c1][c3] + A[c1][c5] * B[c5][c3];
                }
              }
            }
          }
        }
      }
    }
  }
  
#pragma endscop
}
