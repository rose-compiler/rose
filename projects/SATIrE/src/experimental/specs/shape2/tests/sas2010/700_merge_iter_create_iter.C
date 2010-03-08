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

  List *t;
  if (p->d > q->d) {
    t = p;
    p = q;
    q = t;
  }

  List *head = p;
  List *cur  = head;

  p=p->n;

  while (p != NULL && q != NULL) {
    if (p->d > q->d) {
      t = p;
      p = q;
      q = t;
    }
    cur->n = p;
    cur    = cur->n;
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
  List *a = create_iter(6);
  List *b = create_iter(6);

  List *head = merge_iter(a, b);

  return 1;
}

