
struct s 
{
  int b;
  int d;
}
;

int main()
{
  struct s *a;
  struct s *c;
  int i;
  int N;
  int d;
  int toto;
  
#pragma scop
  for (i = 0; i < N; ++i) {
    a -> b = d;
  }
  c -> d = a -> b;
  toto = a -> d;
// Getter/Setter are not supported, only method calls.
// a->d() = bla bla is NOT ok
  
#pragma endscop
}
