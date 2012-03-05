#define NULL ((List*)0)
class List {
public:
  List():next(0) {}
  List* next;
};

int input() {
  return 1;
}

int main() {
  List *a, *b, *x, *y, *v, *w;

  a = new List();
  b = new List();
  v = new List();
  w = new List();
  x = new List();
  y = new List();

  b->next = x;
  x->next = y;

  if (input) {
    a->next = v;
  } else {
    v->next = w;
  }

  /* here it is interesting wether a->v->w and b->x->y are represented differently
   * in SRW: they are not; both are joined to a list
   * in NNH: they are kept apart in two sets; b->x->y is present in both
   * */
  a->next->next = NULL;
  b->next->next = NULL;

  return 0;
}

