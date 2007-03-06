#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *x, *u, *v, *n1;

  u = new N;
  n1 = new N;
  u->sel1  = n1;

  x = new N;
  v = new N;
  x->sel1 = u;
  v->sel1 = u;
  
  //[x.sel:=nil] when #into <= 1
  x->sel1 = NULL;

  return 0;
}
