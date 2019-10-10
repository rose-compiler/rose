int x;
int y;

int main() {

  int c[10];
  #pragma omp parallel for
  for(int i=0;i<10;i++) {
    c[i]=0;
  }
  return 0;
}
