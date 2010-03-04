typedef struct List {
public:
  struct List *n;
  int d;
} List;

#define NULL ((List*)0)

// recursively reverses, recursively appends
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

List* app_recur(List* p, List* q) {
  List* t;

  if (p == NULL) {
    return q;
  }

  t = p->n;
  t = app_recur(t,q);

  p->n = NULL;
  p->n = t;
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
  r = app_recur(y,x);
  return r;
}

int main(int argc, char **argv) {
  List *a = create_d(6);
  rev(a);
  return 0;
}

