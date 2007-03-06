#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *x, *v, *w, *n0, *n1, *t;

  n0 = new N;
  n1 = new N;
  t  = new N;
  n0->sel1 = t;
  t->sel1  = n1;
  t = NULL;

  v = new N;
  w = new N;
  x = new N;
  v->sel1 = x;
  x->sel2 = w;
  
  //[x:=nil]
  x = NULL;

  return 0;
}
