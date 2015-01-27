#include "List.H"

List *create_iter(int n) {
  List *head=0;
  List *newElement;

  while (n>=0) {
    newElement = new List(n);
    newElement->n = head;
    head = newElement;
    n--;
  }

  return head;
}


// merges two ascending sorted lists into one
// RinetzkySagiv01: merge
List* merge_recur(List* p , List* q) {
  List* tail;
  List* head;

  if (p == NULL)
    return q;

  if (q == NULL)
    return p;

  if (p == q)
    return p;

  if (p->d < q->d) {
    tail = merge_recur(p->n,q);
    head = p;
  }
  else {
    tail = merge_recur(p,q->n);
    head = q;
  }

  head->n = tail;
  return head;
}

int main(int argc, char **argv) {
  List *a = create_iter(6);
  List *b = create_iter(6);

  List *head = merge_recur(a, b);

  a->n->n = NULL;
  b->n->n = NULL;
  head->n->n = NULL;

  return 1;
}

