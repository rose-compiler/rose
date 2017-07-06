
int main()
{
  int i;
  int n;
  int c[n];
  
#pragma scop
{
    int c1;
    if (n >= 1) {
      for (c1 = 1; c1 <= n; c1++) {{
// A C++ comment.
          c[c1] = 0;
        }
      }
    }
  }
/* Another comment in the scop. */
  
#pragma endscop
}
