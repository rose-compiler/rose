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


// returns pointer to first element in a
// list with a given key
// RinetzkySagiv01: search
List* search_recur(List* head, int k) {
  if (head == NULL)
    return NULL;

  if (head->d == k)
    return head;

  head = head->n;
  head = search_recur(head,k);
  return head;
}

int main(int argc, char **argv) {
  List *head = create_fixed(6);

  search_recur(head, 3);

  return 1;
}

