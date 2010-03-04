typedef struct List {
public:
  struct List *n;
  int d;
} List;

#define NULL ((List*)0)

// a procedure which creates a list with
// decreasing keys.
// never returns an empty list !
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

// a procedure which reverses a list by
// appending the node to the end of its
// reversed tail

List* rev_app(List* r) {
  List *t;

  if (r == NULL) {
    return NULL;
  }
  t = r->n;
  t = rev_app(t);

  r->n = NULL;

  t = app_recur(t,r);

  return t;
}

int main(int argc, char **argv) {
  List *a = create_d(6);
  rev_app(a);
  return 0;
}

