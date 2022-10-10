struct S2 {
  int var21;
  int var22;
};
struct {
  struct S2 var11;
  struct S2 array12[5];
} s1;

int main() {


  int x;
  struct S2 s21=s1.array12[0];
  s1.var11 = s21;

  int y;
  s1.var11 = s1.array12[0];

}
