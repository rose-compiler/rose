int main() {

  // multi-dim array init wcet_bench: ns.c

  struct s1{
    int a;
    struct ss1{
      int c;
      char d;
    }e;
  }x1[] = {
    1, 2, 'a',
    3, 4, 'b'
  };

  struct s2{
    int a;
    struct ss2{
      int c;
      char d;
    }e;
  }x2[] = {
    {1, {2, 'a'}},
    {3, {4, 'b'}}
  };
  
  // separate array of structs
  s1 a[2]= {
    1, 2, 'a',
    3, 4, 'b'
  };
  return 0;
}
