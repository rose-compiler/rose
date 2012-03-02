#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *nx, *nu, *nv, *n1;

  nx = new N;
  nu = new N;
  nv = new N;
  n1 = new N;

  nx->sel1 = nu;
  nv->sel1 = nu;
  nu->sel1 = n1;
  
  //[x.sel:=nil] when #into <= 1
  nx->sel1 = NULL;

  return 0;
}
