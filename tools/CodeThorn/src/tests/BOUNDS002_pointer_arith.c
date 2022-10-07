
struct S2 {
  char a;
};

struct S1 {
  struct S2 buf[10];
};

struct S1 s1;

int main() {
  struct S2 *ptr = {s1.buf};
  (ptr+1)->a = 0; 
  return 0;
}
