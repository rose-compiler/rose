
int main()
{
  int i;
  int n;
  int a;
  int b;
  int c;
  int d;
  
#pragma scop
{
    int c1;
    if (n >= 1) {
      for (c1 = 0; c1 <= ((1 < n + -1?1 : n + -1)); c1++) {{
          b = 0;
        }
      }
      if (n >= 3) {
        a = 0;
        b = 0;
      }
      for (c1 = 3; c1 <= n + -2; c1++) {{
          a = 0;
          b = 0;
          c = 0;
          d = 0;
        }
      }
      if (n >= 4) {
        a = 0;
        b = 0;
        d = 0;
      }
    }
  }
  
#pragma endscop
}
