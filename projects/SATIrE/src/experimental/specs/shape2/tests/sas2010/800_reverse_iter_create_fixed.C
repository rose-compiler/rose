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
  List *head = create_fixed(6);

  List *revd = reverse_iter(head);

  return 1;
}

