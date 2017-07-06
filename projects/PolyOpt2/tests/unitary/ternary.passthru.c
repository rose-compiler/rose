
int main()
{
  int i;
  int n;
  int b;
  int c;
  int d;
  int a;
  
#pragma scop
{
    int c1;
    if (n >= 1) {
      for (c1 = 0; c1 <= n + -1; c1++) {{
          b = (c < d?a + 1 : 2 * b);
        }
      }
    }
  }
  
#pragma endscop
}
