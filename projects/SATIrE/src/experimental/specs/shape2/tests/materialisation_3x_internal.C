#define NULL ((N*)0)

class N {
public:
  N *next,*sel,*cdr;
};

int main() {
  N *x, *y, *t;

  y = new N;
  t = new N;
  y->next = t;
  t = NULL;

  t = new N;
  t->next = t;
  t = new N;
  t->sel = t;
  t = new N;
  t->cdr = t;
  t = NULL;

  // materialisation with 3 internal edges
  x = y->next;

  return 0;
}
