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

List* app_iter(List* head, List* q) {
  List* cur = head;

  if (head == NULL) {
    return q;
  }

  while (cur->n != NULL) {
    cur = cur->n;
  }

  cur->n = q;

  return head;
}


// reverses a list recursively
// (each node is appended to the end
// of its reversed tail)
// RinetzkySagiv01: rev_app
List* reverse_recur(List* head) {
  List *reversed;

  if (head == NULL) {
    return NULL;
  }

  reversed = reverse_recur(head->n);

  head->n = NULL;
  reversed = app_iter(reversed,head);

  return reversed;
}

int main(int argc, char **argv) {
  List *a = create_iter(6);
  List *b = create_iter(6);

  List *rev_a = reverse_recur(a);

  a->n->n = NULL;
  b->n->n = NULL;
  rev_a->n->n = NULL;

  return 1;
}

