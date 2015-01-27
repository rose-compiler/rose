#include "List.H"

// creates a list with ascending keys
// never returns empty list
// RinetzkySagiv01: create_d
List *create_recur(int n) {

  List *head = new List(5 - n);

  if (n <= 0) {
    return head;
  }

  List *t = create_recur(n-1);
  head->n = t;

  return head;
}


int main(int argc, char **argv) {
  List *a = create_recur(6);
  List *b = create_recur(6);

  a->n->n = NULL;
  b->n->n = NULL;

  return 1;
}

