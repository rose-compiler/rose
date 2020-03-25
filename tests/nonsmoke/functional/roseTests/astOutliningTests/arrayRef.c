//A test based on https://github.com/rose-compiler/rose/issues/83
// Liao, 3/24/2019
int main(int argc, char **argv) {
  float foo[10] = { 0.0 };

#pragma rose_outline
  {
    foo[3] = 4.2;
  }

  return 0;
}
