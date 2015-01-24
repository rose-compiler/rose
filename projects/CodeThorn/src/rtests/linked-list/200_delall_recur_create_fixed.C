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
// RinetzkySagiv01: del_all
void delall_recur(List *head) {
  if (head == NULL) {
    return;
  }

  List *t = head->n;
  delall_recur(t);

  delete head;
  head = NULL;
}

int main(int argc, char **argv) {
  List *a = create_fixed(6);
  List *b = create_fixed(6);

  delall_recur(a);
  delall_recur(b);

  a->n->n = NULL;
  b->n->n = NULL;

  return 1;
}

