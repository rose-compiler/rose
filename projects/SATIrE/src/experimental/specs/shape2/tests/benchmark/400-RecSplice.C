// Rinetzky/Sagiv/Yahav:
// Interprocedural Shape Analysis for Cutpoint-Free Programs

#define NULL ((List*)0)

class List {
public:
  List(int d) : data(d) { }
  List *n;
  int data;
};

List *create3(int k) {
  List *t1 = new List(k);
  List *t2 = new List(k+1);
  List *t3 = new List(k+2);
  t1->n = t2;
  t2->n = t3;
  return t1;
}

List *splice(List *p, List *q) {
  List *w = q;
  if (p != NULL) {
    List *pn = p->n;
    p->n = NULL;
    p->n = splice(q, pn);
    w = p;
  }
  return w;
}

int main(int argc, char **argv) {
  List *x = create3(1);
  List *y = create3(4);
  List *z = create3(7);

  List *t = splice(x, y);
  List *s = splice(y, z);
}

