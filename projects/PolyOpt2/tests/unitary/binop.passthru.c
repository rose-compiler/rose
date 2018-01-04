
int main()
{
  int a;
  int b;
  int n;
  
#pragma scop
{
    int c1;
    if (n >= 1) {
      for (c1 = 0; c1 <= n + -1; c1++) {{
          a += b;
          --a;
          b++;
        }
      }
    }
  }
  
#pragma endscop
}
