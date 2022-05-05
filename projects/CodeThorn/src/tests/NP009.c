struct S2 {
  int var21;
  int var22;
  int* p;
};
struct {
  struct S2 var11;
  struct S2 array12[5];
} s1;

int main() {


  int x;
  s1.var11.p=0;
  *s1.var11.p=0;

}
