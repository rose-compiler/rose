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
  List *a = create_iter(6);
  List *b = create_iter(6);

  delall_recur(a);
  delall_recur(b);

  a->n->n = NULL;
  b->n->n = NULL;

  return 1;
}

