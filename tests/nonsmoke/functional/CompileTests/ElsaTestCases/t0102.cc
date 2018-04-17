typedef int *(*intpa3p_t)[3];
typedef int *intpa3_t[3];

int main() {
  int x = 3;
  intpa3_t f = {&x, &x, &x,};
  intpa3p_t g = &f;
  int y = *(*g)[1];
  return 0;
}
