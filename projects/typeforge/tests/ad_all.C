double init = 0.0;
double noinit;

int main() {
  double a = 0.0;
  a = a+2;
#pragma adapt begin
  a += 10.0;
  a -= 1.0;
  a *= 2.0;
  a /= 3.0;
  double arrayDoubles[5];
  arrayDoubles[1] = 0.0;
#pragma adapt output arrayDoubles 10-6
#pragma adapt end
  return 0;
}
