
int main()
{
  int Ca;
  int b;
  int N;
  int M;
  
#pragma scop
{
    int c5;
    int c3;
    int c1;
    if (N >= 1) {
      if (M >= 1) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            for (c3 = 0; c3 <= M + -1; c3++) {{
                for (c5 = 0; c5 <= N + -1; c5++) {{
                    Ca = 0;
                  }
                }
              }
            }
            for (c3 = 0; c3 <= N + -1; c3++) {{
                b = 0;
              }
            }
          }
        }
      }
      if (M <= 0) {
        for (c1 = 0; c1 <= N + -1; c1++) {{
            for (c3 = 0; c3 <= N + -1; c3++) {{
                b = 0;
              }
            }
          }
        }
      }
    }
  }
  
#pragma endscop
}
