typedef struct List {
public:
  struct List *n;
  int d;
} List;

#define NULL ((List*)0)

// a procedure which creates a list with
// decreasing keys.
// may return an empty list!
List* create_u(int k) {
  List* a;
  List* t;

  if (k <= 0)
    return NULL;

  t = create_u(k-1);
  a = new List();
  a->n = t;
  a->d = k;

  return a;
}

int main(int argc, char **argv) {
  List *a = create_u(4);
  return 0;
}

