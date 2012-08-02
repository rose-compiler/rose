#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2,*sel3;
};

int main() {
  N *nx, *ny, *nv, *nw, *n0, *n1, *na, *nb, *nc;

  nx = new N;
  n0 = new N;
  n1 = new N;
  n0->sel1 = nx;
  nx->sel1 = n1;

  nv = new N;
  nw = new N;
  ny = new N;
  na = new N;
  nb = new N;
  nc = new N;
  ny->sel1 = na;
  na->sel2 = nw;
  
  nv->sel1 = nb;
  nc->sel3 = nc;

  na = NULL;
  nb = NULL;
  nc = NULL;

  //[x:=y.sel] case 3
  nx = ny->sel1;

  // add this expression to test for may-aliasing
  // ny->sel1->sel2 == nw (in 3 of the 6 cases)
  ny->sel1->sel2 = NULL;

  return 0;
}
