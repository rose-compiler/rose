typedef struct listru {
	struct listru *next;
	} *list;

list reverse(list head0) {
  list head;
  list h;
  list g;
  head = head0;
  while(head!=0) {
    g = head->next;
    head->next=h;
    h = head;
    head=g;
    g = 0;
  }
  h=0;
  return head;
}

list create() {
  list chead;
  list g;
  chead = 0;
  int n=0;
  while(n>0) {
    g = new listru;
    g->next= chead; // insert at start of list
    chead = g;
    g=0;
    n--;
  }
  return chead;
}

int main() {
  list l1;
  list l2;
  l1=create();
  l2=reverse(l1);
  return 0;
}

