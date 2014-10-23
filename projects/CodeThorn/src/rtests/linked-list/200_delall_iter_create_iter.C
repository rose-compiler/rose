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
  List *a = create_iter(6);
  List *b = create_iter(6);

  delall_iter(a);
  delall_iter(b);

  a->n->n = NULL;
  b->n->n = NULL;

  return 1;
}

