
int main()
{
  int i;
  int j;
  int n;
  int a;
  int c;
  
#pragma scop
{
    int c3;
    int c1;
    if (n >= 1) {
      for (c1 = 0; c1 <= n + -1; c1++) {{
          for (c3 = 0; c3 <= 1; c3++) {{
              a;
            }
          }
        }
      }
      for (c1 = 0; c1 <= n + -1; c1++) {{
          c;
        }
      }
    }
  }
  
#pragma endscop
}
