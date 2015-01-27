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
// PavluSchordanKrall10: new testcase
List* remove_iter(List* head, int k) {
  List* prev;
  List* cur = head;

  if (cur->d == k) {
    head = cur->n;
    delete cur;
    cur = NULL;
    return head;
  }

  prev = head;
  cur  = head->n;
  while (cur != NULL) {

    if (cur->d == k) {
      prev->n = cur->n;
      delete cur;
      cur = NULL;
      return head;
    }
    prev = cur;
    cur  = cur->n;
  }

  return head;
}

int main(int argc, char **argv) {
  List *a = create_fixed(6);
  List *b = create_fixed(6);

  remove_iter(a, 4);

  a->n->n->n = NULL;
  b->n->n->n = NULL;

  return 1;
}

