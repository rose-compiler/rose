int main() {
  double a[10];
#pragma omp parallel for
    for(int i=1;i<4;i++) {
      a[i]=a[i+1];
    }
    return 0;
}
