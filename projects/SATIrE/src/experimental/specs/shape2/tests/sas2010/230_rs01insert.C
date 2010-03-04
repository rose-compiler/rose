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

// a procedure which inserts an element
// into an ordered list.
List* insert(List* h, int k) {
  List* t;

  if (h != NULL && h->d < k) {
    t = h->n;
    t = insert(t,k);
    h->n = NULL;
    h->n = t;
    return h;
  }

  t = new List();
  t->n = h;
  t->d = k;
  return t;
}

int main(int argc, char **argv) {
  List *a = create_d(6);
  insert(a, 2);
  insert(a, 4);
  return 0;
}

