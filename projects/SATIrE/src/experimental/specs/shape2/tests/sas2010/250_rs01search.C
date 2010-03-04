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

// a procedure which returns a pointer
// to an element with a given key.

List* search(List* h, int k) {
  if (h == NULL)
    return NULL;

  if (h->d == k)
    return h;

  h = h->n;
  h = search(h,k);
  return h;
}

int main(int argc, char **argv) {
  List *a = create_d(6);
  search(a,4);
  return 0;
}

