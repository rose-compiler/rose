typedef struct L {
	struct L *next;
  int val;
} L;

int main() {
  L *a = new L();
  L *b = new L();
  L *x = new L();

  a->next = x;
  b->next = x;
  x = 0;
}
