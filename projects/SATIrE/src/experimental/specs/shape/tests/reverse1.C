typedef struct listru {
	struct listru *next;
	} *list;
	list h,g,head;

list create() {
  list myhead=0;
  while(1) {
    g = new listru;
    g->next= myhead;
    myhead = g;
    g=0;
  }
  return myhead;
}

void reverse() {
	h = 0;
	while(head!=0) {
		g = head->next;
		head->next=h;
		h = head;
		head=g;
		g = 0;
	}
	h=0;
	return;
}

int main() {
  head = 0;
  head=create();
  reverse();
  return 0;
}
