extern void stat ();
struct stat {};
void f() {
  struct stat st;
}
