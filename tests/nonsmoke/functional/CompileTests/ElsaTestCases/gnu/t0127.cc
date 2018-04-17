// test typeof
typedef unsigned int size_t;
void *malloc(size_t size);
int main() {
  // a typical use of typeof
  int sz = 16;
  char *a = (typeof(a)) malloc(sz * sizeof *a);

  // weird use
  typeof(*a) x = 'c';

  // typeof also applies to types; that is, it is idempotent
  typeof(int) y = 5;
  typeof(typeof(y)) z = 6;

  return 0;
}
