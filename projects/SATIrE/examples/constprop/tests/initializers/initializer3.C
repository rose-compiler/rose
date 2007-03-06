// global definition of struct

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

int main() {
  
  struct s1 a[][2]= {
    {{11,{111,'a'}},{12,{121,'b'}}},
    {{21,{211,'c'}},{22,{221,'d'}}},
    {{31,{311,'e'}},{32,{321,'f'}}}
  };

  int y1=a[1][2].a;
  int y2=a[1][2].e.c;
  a[1][1].a=y2;
  a[1][2].a=a[2][1].e.c;
  (a[1])[2].s1::a = ((a[2])[1].s1::e.s1::ss1::c);

  return 0;
}
