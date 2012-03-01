#define NULL ((List*)(0))

class List {
  public:
    List() : next(NULL), d(0) {}
  class List *next;
  int d;
};

int main(int argc, char **argv) {

  List *suml = new List();
  List *p = suml;
  while (1) {
    p->next = new List();
    p = p->next;
  }
  p = NULL;

  List *t = new List();
  t->next = suml;
  suml = NULL;

  List *y = t;
  List *x = t;
  t = NULL;


  x = x->next;

  p = NULL;

  return 0;
}
