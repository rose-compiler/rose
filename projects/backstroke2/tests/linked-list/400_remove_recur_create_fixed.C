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


// deletes from an unsorted list
// RinetzkySagiv01: del
List* remove_recur(List* head, int k) {
  List* t;

  if (head == NULL)
    return NULL;

  t = head->n;

  if (head->d == k) {
    head->n = NULL;
    delete head;
    head = NULL;
    return t;
  }

  t = remove_recur(t,k);
  head->n = t;
  return head;
}

int main(int argc, char **argv) {
  List *a = create_fixed(6);
  List *b = create_fixed(6);

  remove_recur(a, 4);

  a->n->n->n = NULL;
  b->n->n->n = NULL;

  return 1;
}

