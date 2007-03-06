#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2,*sel3;
};

int main() {
  N *x, *y, *t, *v, *w, *n0, *n1;

  x = new N;
  n0 = new N;
  n1 = new N;
  n0->sel1 = x;
  x->sel1  = n1;

  t = new N;
  v = new N;
  w = new N;
  y = new N;
  y->sel1 = t;

  // before adding more in-edges, change t
  // into summary node to avoid introducing
  // of sharing. this needs to be done as
  // the example in the nielson slides has
  // {} unshared (supposedly to avoid too
  // large a number of resulting graphs)

  t = NULL;

  t = new N;
  v->sel1 = t;
  t = NULL;

  t = new N;
  t->sel2 = w;
  t->sel3 = t;
  t = NULL;

  //[x:=y.sel] case 3
  x = y->sel1;

  return 0;
}
