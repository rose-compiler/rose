struct s {
  int b;
  int d;
};
  
int main()
{
  int i, N, d;
  struct s a;
  struct s c;
#pragma scop
  for (i = 0; i < N; ++i) {
    a.b = d;
  }
  c.d = a.b;
#pragma endscop
}
