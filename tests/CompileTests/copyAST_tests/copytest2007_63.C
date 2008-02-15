void f() {
l:
  goto l;
}

#if 0
void g() {
  f();
  f();
}
#endif
