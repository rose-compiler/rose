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
// PavluSchordanKrall10: new testcase
List* merge_iter(List* p , List* q) {
  if (p == NULL) {
    return q;
  }
  if (q == NULL) {
    return p;
  }
  if (p->d > q->d) {
    return merge_iter(q, p);
  }

  List *t;
  List *head = p;
  List *cur  = head;

  p=p->n;

  while (p != NULL && q != NULL) {
    if (p->d > q->d) {
      t = p;
      p = q;
      q = t;
      t = NULL;
    }
    cur->n = p;
    cur    = p;
    p      = p->n;
  }

  if (p == NULL) {
    cur->n = q;
  }
  else {
    cur->n = p;
  }

  return head;
}

int main(int argc, char **argv) {
  List *p = create_iter(6);
  List *q = create_iter(6);

  List *head = merge_iter(p, q);

  p->n->n = NULL;
  q->n->n = NULL;
  head->n->n = NULL;

  return 1;
}

