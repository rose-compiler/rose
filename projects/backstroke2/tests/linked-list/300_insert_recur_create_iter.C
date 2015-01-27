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


// inserts an element into an ascending
// ordered list
// RinetzkySagiv01: insert
List* insert_recur(List* head, int k) {
  if (head != NULL && head->d < k) {
    List *tail = head->n;
    tail = insert_recur(tail,k);
    head->n = tail;
  }
  else {
    List *tmp;
    tmp = new List(k);
    tmp->n = head;
    head = tmp;
    tmp = NULL;
  }
  return head;
}

int main(int argc, char **argv) {
  List *a = create_iter(6);
  List *b = create_iter(6);

  insert_recur(a, 3);
  insert_recur(b, 4);

  a->n->n = NULL;
  b->n->n = NULL;

  return 1;
}

