// Test suggested by Alin Jula

int x;
int w() {
  x = 6;
  int x;
  x = 7;
  return x;
}

int main(int, char**) {
  int z = w();
  return !(x == 6 && z == 7);
}
