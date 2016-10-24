struct B {
  B(int);
};
int main() {
  //ERROR(1): B b;      // no default ctor
  return 0;
}
