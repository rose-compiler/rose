int x;
int w() {
  x = 5;
  {
    x = 6;
    int x;
    x = 7;
  }
  int x;
  x = 8;
  return x;
}

int main(int, char**) {
  int z = w();
  return !(x == 6 && z == 8);
}
