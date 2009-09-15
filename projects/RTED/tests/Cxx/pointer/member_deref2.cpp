int main() {
  int* p = new int;
  *p = 42;
  
  //illegal write
  *((int*) *p) = 42;
    
  return 0;
}
