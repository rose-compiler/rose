int main() {
  int a=0;
  for (int  i=0; i<10; i++) {
    a=a+i;
    a+=i;
  }
  return !(a/2==10);
}

  
  
  
