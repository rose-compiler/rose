#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *x, *y, *u, *v, *w, *n0, *n1;

  x = new N;
  n0 = new N;
  n1 = new N;
  n0->sel1 = x;
  x->sel1  = n1;

  u = new N;
  v = new N;
  w = new N;
  y = new N;
  y->sel1 = u;
  v->sel1 = u;
  u->sel2 = w;
  
  //[x:=y.sel] case 2
  x = y->sel1;

  return 0;
}
