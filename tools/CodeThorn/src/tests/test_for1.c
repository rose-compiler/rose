// we intentionally avoid using ++,-- in this test
int main() {
  int sum=0;
  for(int i=1;i<10;i=i+1) {
    sum=sum+i;
  }
  return 0;
}
