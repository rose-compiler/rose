#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *n0, *nv, *nx, *nw, *t0, *t1;

  n0 = new N;
  nv = new N;
  nx = new N;
  nw = new N;
  t0 = new N;
  t1 = new N;

  nv->sel1 = nx;
  t0->sel1 = n0;
  n0->sel1 = t1;
  nx->sel2 = nw;
  n0 = NULL;

  //[x:=nil]
  nx = NULL;

  return 0;
}
