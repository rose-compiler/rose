#define NULL ((List*)0)
class List {
public:
  List() : sel(NULL), sel2(NULL) { }
  List* sel;
  List* sel2;
};

int main() {
  List *k, *x, *y;
  List *t;

  k = new List();
  x = new List();
  y = new List();

  x->sel2 = new List();
  y->sel2 = new List();

  x->sel = k;
  y->sel = k;
  k->sel = new List();

  x->sel->sel = NULL;
  y->sel->sel = NULL;

  return 0;
}
