#include "List.H"

// creates a list with ascending keys
// SagivRepsWilhelm98: create
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


int main(int argc, char **argv) {
  List *head = create_iter(6);

  return 1;
}

