#define NULL ((N*)0)

class N {
public:
  N *next,*sel,*cdr;
};

int main() {
  N *x, *y, *z, *t;

  // going-in edge
  y = new N;
  t = new N;
  y->next = t;
  t = NULL;

  // going-in edge
  z = new N;
  t = new N;
  z->next = t;
  t = NULL;

  // backedge
  t = new N;
  t->next = t;
  t = NULL;

  // backege
  t = new N;
  t->sel = t;
  t = NULL;

  //backedge
  t = new N;
  t->cdr = t;
  t = NULL;

  // materialisation with 3 internal edges
  x = y->next;

  return 0;
}
