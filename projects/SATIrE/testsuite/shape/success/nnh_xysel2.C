#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *nx, *ny, *nu, *nv, *nw, *n0, *n1;

  nx = new N;
  n0 = new N;
  n1 = new N;
  n0->sel1 = nx;
  nx->sel1 = n1;

  nu = new N;
  nv = new N;
  nw = new N;
  ny = new N;
  ny->sel1 = nu;
  nv->sel1 = nu;
  nu->sel2 = nw;

  //[x:=y.sel] case 2
  nx = ny->sel1;

  return 0;
}
