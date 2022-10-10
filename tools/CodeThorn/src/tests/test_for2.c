// we intentionally avoid using ++,-- in this test
int main() {
  int sum=0;
  for(int i=1;i<3;i=i+1) {
    for(int j=sum,k=j;j<3;j=j+1) {
      sum=sum+i;
    }
  }
  return 0;
}
