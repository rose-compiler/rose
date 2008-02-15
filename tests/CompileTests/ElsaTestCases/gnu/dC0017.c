int *tableHeader;
typedef int INT;
int i;
void f() {
  __alignof__(*tableHeader);

  __alignof__(INT /*type*/);
  __alignof__(i /*expr*/);
}
