
struct s 
{
  int b;
  int d;
}
;

int main()
{
  int N;
  int d;
  struct s a;
  struct s c;
  
#pragma scop
{
    int c1;
    for (c1 = 0; c1 <= N + -1; c1++) {{
        a . b = d;
      }
    }
    c . d = a . b;
  }
  
#pragma endscop
}
