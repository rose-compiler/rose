#define NULL ((N*)0)

class N {
public:
  N *next;
};

int main() {
  N *x, *y, *z;
  N *px,*qx, *py,*qy, *pz,*qz;

  x = new N;
  px = new N;
  qx = new N;
  px->next = x;
  qx->next = x;

  y = new N;
  py = new N;
  qy = new N;
  py->next = y;
  qy->next = y;

  z = new N;
  pz = new N;
  qz = new N;
  pz->next = z;
  qz->next = z;

  N *print = new N;

  return 0;
}
