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
  List *head = create_iter(6);

  remove_recur(head, 4);

  return 1;
}

