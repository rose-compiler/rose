#define NULL ((N*)0)

class N {
public:
  N *sel1,*sel2;
};

int main() {
  N *nx, *ny, *nu;

  nx = new N;
  nu = new N;
  nx->sel1 = nu;

  ny = new N;
  
  //[x.sel:=y] when #into <= 1
  nx->sel1 = ny;

  return 0;
}
