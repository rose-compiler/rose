struct {
  char a[10];
} s;

void main() {
  char* p = &s.a[0];
  *(p-p)='x';
}
