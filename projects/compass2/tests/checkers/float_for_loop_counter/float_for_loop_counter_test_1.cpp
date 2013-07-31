// 4 Matches

int main() {
  float f;
  double d;
  for (f = 0; f < 3.14; ++f) {}
  for (d = 0; d < 3.14; ++f) {}
  for (float a = 0; a < 3.14; ++a) {}
  for (double b = 0; b < 3.14; ++b) {}
  return 0;
}
