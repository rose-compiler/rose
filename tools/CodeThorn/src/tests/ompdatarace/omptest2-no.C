int x;
int y;

int main() {

  int c[10];
  int tmp;
#pragma omp parallel for private(tmp)
  for(int i=0;i<9;i++) {
    tmp=c[i];
    c[i]=tmp;
  }
  return 0;
}
