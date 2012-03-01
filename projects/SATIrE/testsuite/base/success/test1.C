typedef struct listEl {
	struct listEl* next;
	} *list;

int main() {
  list x;
  list y;
  list z;
  list a;
  list b;
  list c;

  x = new listEl;
  y = x;
  z = new listEl;
  y->next=z;
  x = 0;
  z = 0;
  b = new listEl;
  c = new listEl;
  b->next=c;
  y->next=b;
  a=b;
  c=0;
  a=c;
  b=c;
  b=y->next;
  y->next=0;
  if(0) {
    b->next=0; 
    a=b->next;
  } else {
    a=b;
    b->next=a; // create cycle
  } 
  0; // shows merged result from both branches on edge
  a=0;
  y->next=b;
}
