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

// a procedure which deletes an element
// from an keyed list.
List* del(List* h, int k) {
  List* t;

  if (h == NULL)
    return NULL;

  t = h->n;

  if (h->d == k) {
    h->n = NULL;
    delete h;
    h = NULL;
    return t;
  }

  t = del(t,k);
  h->n = NULL;
  h->n = t;
  return h;
}

int main(int argc, char **argv) {
  List *a = create_d(6);
  del(a, 2);
  del(a, 4);
  return 0;
}

