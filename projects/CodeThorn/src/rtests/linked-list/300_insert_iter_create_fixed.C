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
// SagivRepsWilhelm98: insert
// changed second parameter:
//   was: pointer to existing elem
//   now: key used to create new elem
// changed return type:
//   was: void
//   now: List* to head of list
List* insert_iter(List* head, int k) {
  List* cur;
  List* tail;
  cur = head;
  while (cur->n != NULL && (cur->n->d < k)) {
    cur = cur->n;
  }
  tail = cur->n;

  List *elem = new List(k);
  elem->n = tail;
  cur->n  = elem;

  return head;
}

int main(int argc, char **argv) {
  List *a = create_fixed(6);
  List *b = create_fixed(6);

  insert_iter(a, 3);
  insert_iter(b, 4);

  a->n->n = NULL;
  b->n->n = NULL;

  return 1;
}

