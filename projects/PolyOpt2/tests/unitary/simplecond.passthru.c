
int main()
{
  int n;
  int a;
  
#pragma scop
{
    int c1;
    if (n >= 3) {
      for (c1 = 2; c1 <= n + -1; c1++) {{
          a = 0;
        }
      }
    }
  }
  
#pragma endscop
}
