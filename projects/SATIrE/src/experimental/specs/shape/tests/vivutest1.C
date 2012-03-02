typedef struct listEl {
	struct listEl* next;
	} *list;

int main() {
  list x;
  list e;
  list a;

  x = new listEl;
  a=x;
  while(0) {
    e = new listEl;
    x->next=e;
    x=x->next;
    e=0;
  }
  x=0;
  return 0;
}
