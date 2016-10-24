struct C {
  operator int();
};
void g(int, int);
void g(int, double);
int main() {
  C c;
  g(c, 1);
}
