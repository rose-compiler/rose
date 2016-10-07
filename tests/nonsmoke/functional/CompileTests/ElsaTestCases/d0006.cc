struct C {
  static int strcmp (char *s1, char *s2);
  static int strcmp (int *s1, int *s2);
};
void f () {
  int *k;
  int m[1];
  C::strcmp(k, m);
}
