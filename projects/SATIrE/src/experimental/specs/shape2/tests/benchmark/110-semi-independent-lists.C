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
  List *a, *b;
  List *t;

  a = new List();
  t = new List();
  a->next = t;
  t->next = new List();
  t = NULL;

  b = new List();
  t = new List();
  b->next = t;
  t->next = new List();
  t = NULL;

  if (input()) {
    t = a->next;
    b->next = t;
  }


  a->next->next->next = NULL;
  b->next->next->next = NULL;

  return 0;
}
