class List {
  public: List() {}
  List *next;
};

#define NULL ((List*)0)

int main(int argc, char **argv) {

  List *a = new List();
  List *b = new List();
  List *c = new List();
  List *target = new List();
  List *dim;

  List *x = a;
  x->next = b;
  b->next = c;

  dim = NULL;
  x->next->next = target;
  //
  //
  dim = NULL;

  return 0;
}

