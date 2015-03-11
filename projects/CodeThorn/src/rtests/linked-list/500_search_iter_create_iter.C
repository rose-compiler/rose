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


// returns pointer to first element in a
// list with a given key
// PavluSchordanKrall10: new testcase
List* search_iter(List* head, int k) {
  while (head != NULL) {
    if (head->d == k) {
      return head;
    }
    head = head->n;
  }

  return NULL;
}

int main(int argc, char **argv) {
  List *a = create_iter(6);
  List *b = create_iter(6);

  search_iter(a, 3);

  a->n->n->n = NULL;
  b->n->n->n = NULL;

  return 1;
}

