#include <cstdlib>

int main() {
  int x;
  long a[5];
  long* p;
  p=a;
  char* q1;
  short* q2;
  q1=(char*)malloc(20);
  //q2=new short[10];
  free(q1);
  return 0;
}
