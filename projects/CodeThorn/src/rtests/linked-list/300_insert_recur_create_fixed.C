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


// inserts an element into an ascending
// ordered list
// RinetzkySagiv01: insert
List* insert_recur(List* head, int k) {
  List* t;

  if (head != NULL && head->d < k) {
    t = head->n;
    t = insert_recur(t,k);
    head->n = t;
    return head;
  }

  t = new List(k);
  t->n = head;
  return t;
}

int main(int argc, char **argv) {
  List *a = create_fixed(6);
  List *b = create_fixed(6);

  insert_recur(a, 3);
  insert_recur(b, 4);

  a->n->n = NULL;
  b->n->n = NULL;

  return 1;
}

