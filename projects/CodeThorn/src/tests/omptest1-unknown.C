int x;
int y;

int main() {

  int c[10];
  #pragma omp parallel for
  for(int i=0;i<10;i++) {
    if(c[i]) {
      c[i]=0;
    } else {
      c[i]=1;
    }
  }
  return 0;
}
