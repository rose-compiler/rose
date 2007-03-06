#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *x, *y, *u;

  x = new N;
  u = new N;
  x->sel1 = u;

  y = new N;
  
  //[x.sel:=y] when #into <= 1
  x->sel1 = y;

  return 0;
}
