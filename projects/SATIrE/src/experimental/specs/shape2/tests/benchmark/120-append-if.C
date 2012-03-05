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
  List *p, *x, *y;

  p = new List();
  x = new List();
  y = new List();

  p->next = new List();

  // ----------
  if (input()) {
    x->next = p;
  } else {
    y->next = p;
  }
  // ----------

  x->next->next->next = NULL;
  y->next->next->next = NULL;

  return 0;
}
