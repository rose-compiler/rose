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

// a procedure which deletes all
// elements of a list.
void del_all(List* h) {
  List* t;

  if (h == NULL)
    return;

  t = h->n;
  del_all(t);
  delete h;
  h = NULL;
}


int main(int argc, char **argv) {
  List *a = create_d(4);
  del_all(a);
  return 0;
}

