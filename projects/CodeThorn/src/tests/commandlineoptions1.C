int main(int argc, char* argv[]) {
  char* p1=0;
  char* p2=0;
  if(argc>0) {
    p1=argv[1];
    if(argc>1) {
      p2=argv[2];
    }
  }
  //argv[1][1]=500;
  char* p3;
  p3=argv[1];
  *(p3+1)=100;
  p3[1]=p3[1]+1;
  p3++;
  p3[1]=500;
  return p1!=0 && p2!=0;
}
