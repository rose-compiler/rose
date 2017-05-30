
int main()
{
  int i;
  int n;
  int a;
  
#pragma scop
{
    int c1;
    if (n >= 1) {
      for (c1 = 0; c1 <= n + -1; c1++) {{
          a = 0;
        }
      }
    }
  }
  
#pragma endscop
}
