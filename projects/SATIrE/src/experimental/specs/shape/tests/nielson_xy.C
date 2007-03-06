#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *x, *y, *v, *w, *n0, *n1;

  x = new N;
  n0 = new N;
  n1 = new N;
  n0->sel1 = x;
  x->sel1  = n1;

  v = new N;
  w = new N;
  y = new N;
  v->sel1 = y;
  y->sel2 = w;
  
  //[x:=y] where x != y
  x = y;

  return 0;
}
