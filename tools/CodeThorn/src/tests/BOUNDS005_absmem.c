#define ABS_MEM_LOC 0x10000
short var;
void main() {
  short *ptr;
  var = 0;
  ptr = ABS_MEM_LOC+0x1002;
  *ptr = var;
}
