int main() {
  int* p = new int;
  *p = 42;
  int k =*p;
  //illegal write
  *((int*) *p) = 42;
    
  return 0;
}
