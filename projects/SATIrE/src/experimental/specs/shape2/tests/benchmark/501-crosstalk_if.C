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
  List *x = new List();
  List *y = new List();
  List *lst = new List();
  List *p = new List();

  x->next = p;
  y->next = p;

  List *t = new List();
  lst->next = t;
  t->next = new List();
  t = NULL;

  // initiate crosstalk
  p = NULL;

  x->next->next = NULL;
  y->next->next = NULL;

  return 0;
}
