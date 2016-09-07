int main() {
  int m1 = 1, m2 = 3, m4 = 4;

  int m6 = (m4 = 1);

  int &mr = m2;
  int m5 = 1;
  int i = 0;
  int* ip = &i;
  if(2 >= 10) {
    int i2 = 1;
    mr = 1;
    m5 = m2; // mr becomes live here too because it is an alias for m2!
  }
  else {
    int i3 = 1;
    int i4 = 1;
    *ip = 1;
    i3 = 1;
    i3 = i4;
    i3 = m1;
  }
  if(1) {

  }
  int k = m5;
  1;
}
