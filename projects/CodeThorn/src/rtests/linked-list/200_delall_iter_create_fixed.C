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


// deletes all elements of a list
// PavluSchordanKrall10: new testcase
void delall_iter(List *head) {
  List *t;

  while (head != NULL) {
    t = head->n;
    delete head;
    head = NULL;
    head = t;
  }
}

int main(int argc, char **argv) {
  List *a = create_fixed(6);
  List *b = create_fixed(6);

  delall_iter(a);
  delall_iter(b);

  a->n->n = NULL;
  b->n->n = NULL;

  return 1;
}

