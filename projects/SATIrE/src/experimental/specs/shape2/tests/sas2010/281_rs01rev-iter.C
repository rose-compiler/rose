typedef struct List {
public:
  struct List *n;
  int d;
} List;

#define NULL ((List*)0)

// recursively revereses, procedurally appends
List* create_d(int k) {
  List* a;
  List* t;

  a = new List();
  a->d = k;

  if (k <= 0)
    return a;

  t = create_d(k-1);
  a->n = t;

  return a;
}

List* app_iter(List* p, List* q) {
  List* t1;
  List* t2;

  if (p == NULL) {
    return q;
  }

  t1 = p;
  t2 = t1->n;

  while (t2 != NULL) {
    t1 = t2;
    t2 = t2->n;
  }

  t1->n = q;
  return p;
}

List* rev(List* x) {
  List* y;
  List* t;
  List* r;

  if (x == NULL)
    return NULL;

  t = x->n;
  x->n = NULL;

  y = rev(t);
  r = app_iter(y,x);
  return r;
}

int main(int argc, char **argv) {
  List *a = create_d(6);
  rev(a);
  return 0;
}


