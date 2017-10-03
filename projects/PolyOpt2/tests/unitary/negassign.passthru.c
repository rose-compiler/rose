
int main()
{
  int N;
  int alpha;
  int sum;
  
#pragma scop
{
    int c1;
    if (N >= 2) {
      for (c1 = 1; c1 <= N + -1; c1++) {{
          alpha = -sum;
        }
      }
    }
  }
  
#pragma endscop
}
