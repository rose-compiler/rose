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


// reverses a list iteratively
// (each node is inserted as
// the new head of the reversed list)
// SagivRepsWilhelm98: reverse
List* reverse_iter(List* x) {
  List* y;
  List* t;
  y=0;
  while (x!=0) {
    t=y;
    y=x;
    x=x->n;
    y->n=t;
  }
  t=0;
  return y;
}


int main(int argc, char **argv) {
  List *a = create_iter(6);
  List *b = create_iter(6);

  List *rev_a = reverse_iter(a);

  a->n->n = NULL;
  b->n->n = NULL;
  rev_a->n->n = NULL;

  return 1;
}

