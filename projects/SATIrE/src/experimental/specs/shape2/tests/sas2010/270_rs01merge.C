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

// merges two descending sorted lists into one
// descending sorted list
List* merge(List* p , List* q) {
  List* temp;
  List* tail;
  List* head;

  if (p == NULL)
    return q;

  if (q == NULL)
    return p;

  // vpavlu: added this, so that merge(a,a) does terminate
  if (p == q)
    return p;

  if (p->d > q->d) {
    temp = p->n;
    tail = merge(temp,q);
    head = p;
  }
  else {
    temp = q->n;
    tail = merge(p,temp);
    head = q;
  }

  head->n = NULL;
  head->n = tail;

  return head;
}

int main(int argc, char **argv) {
  List *a = create_d(6);
  List *b = create_d(4);
  merge(a, b);
  return 0;
}

