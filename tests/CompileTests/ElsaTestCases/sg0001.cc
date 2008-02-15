class A {};
int main() {
  const A *val = 0;
  val->~A ();
}
