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
  bool skip_elem = false;
  while (cur != NULL) {
    if (cur->d == k) {
      skip_elem = true;
      break;
    }
    prev = cur;
    cur  = cur->n;
  }

  if (skip_elem) {
    prev->n = cur->n;
    delete cur;
    cur = NULL;
  }

  return head;
}

/*
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
  bool skip_elem = false;
  while (cur != NULL) {
    if (cur->d == k) {
      skip_elem = true;
      break;
    }
    prev = cur;
    cur  = cur->n;
  }

  if (skip_elem) {
    prev->n = cur->n;
    delete cur;
    cur = NULL;
  }

  return head;
}
*/

int main(int argc, char **argv) {
  List *a = create_iter(6);
  List *b = create_iter(6);

  remove_iter(a, 4);

  a->n->n->n = NULL;
  b->n->n->n = NULL;

  return 1;
}

