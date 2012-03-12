#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2,*sel3;
};

int main() {
  N *nx, *ny, *nv, *nw, *n0, *n1, *nn;

  nx = new N;
  n0 = new N;
  n1 = new N;
  n0->sel1 = nx;
  nx->sel1 = n1;

  nv = new N;
  nw = new N;
  ny = new N;
  nn = new N;
  ny->sel1 = nn;
  nv->sel1 = nn;
  nn->sel2 = nw;
  nn->sel3 = nn;
  nn = NULL;
  
  //[x:=y.sel] case 3, with shared {}
  nx = ny->sel1;

  return 0;
}
