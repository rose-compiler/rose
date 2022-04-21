struct BUFFER {
  char a;
};

struct HOLDER {
  struct BUFFER BUF[10];
};

struct HOLDER HELD;

int main() {
  struct BUFFER *ptr = {HELD.BUF};
  (ptr+1)->a = 0; 
  return 0;
}
