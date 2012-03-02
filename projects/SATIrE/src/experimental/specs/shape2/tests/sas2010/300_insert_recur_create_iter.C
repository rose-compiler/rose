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
  List *head = create_iter(6);

  insert_recur(head, 3);

  return 1;
}

