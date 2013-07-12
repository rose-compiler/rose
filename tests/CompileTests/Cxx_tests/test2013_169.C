// Bug report from Jason Dagit, which seems to work fine...
struct S0 { int  f0; };
void foo() {
  struct S0 j = {0};
  +j.f0;
}
