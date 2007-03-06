#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *nx, *ny, *nv, *nw, *n0, *n1;

  nx = new N;
  ny = new N;
  nv = new N;
  nw = new N;
  n0 = new N;
  n1 = new N;
  
  n0->sel1 = nx;
  nx->sel1 = n1;

  nv->sel1 = ny;
  ny->sel2 = nw;
  
  //[x:=y] where x != y
  nx = ny;

  return 0;
}
