int x;
int y;

int main() {

  int c[10];
  #pragma omp parallel for
  for(int i=0;i<9;i++) {
    c[i]=c[i+1];
  }
  return 0;
}
