// t0124.cc
// statement expressions

int main() {
  int x;
  x = ({int y=1; int z=2; y+z;});
}
