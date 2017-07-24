
int main()
{
  int a;
  int b;
  int c;
  int n;
  int i;
  int d[n];
  
#pragma scop
{
    int c1;
    if (n >= 1) {
      for (c1 = 0; c1 <= n + -1; c1++) {{
          a ^= (b | c + 2) % 2 - !(d[n] / 2);
        }
      }
    }
  }
  
#pragma endscop
}
