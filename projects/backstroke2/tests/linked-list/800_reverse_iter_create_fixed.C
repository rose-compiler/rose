#include "List.H"

List *create_fixed(int n) {
  List *head=0;
  List *newElement;

  newElement = new List(n);
  newElement->n = head;
  head = newElement;

  newElement = new List(n);
  newElement->n = head;
  head = newElement;

  newElement = new List(n);
  newElement->n = head;
  head = newElement;

  newElement = new List(n);
  newElement->n = head;
  head = newElement;

  newElement = new List(n);
  newElement->n = head;
  head = newElement;

  newElement = new List(n);
  newElement->n = head;
  head = newElement;

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
  List *a = create_fixed(6);
  List *b = create_fixed(6);

  List *rev_a = reverse_iter(a);

  a->n->n = NULL;
  b->n->n = NULL;
  rev_a->n->n = NULL;

  return 1;
}

