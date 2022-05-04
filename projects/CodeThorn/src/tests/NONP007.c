struct S3 {
  int c;
  int *p;
};

struct S2 {
  int b;
  struct S3 s3;
};

struct S1 {
  int a;
  struct S2 s2;
};

int main() {
  struct S1 s1;
  s1.s2.s3.p=0;
  // *s1.s2.s3.p;
}
