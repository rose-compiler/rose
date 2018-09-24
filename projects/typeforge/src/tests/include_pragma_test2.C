double a = 0.0;
double b = 1.0;

int main(){
#pragma adapt begin
  double x = 0.0;
  double y = 1.0;
#pragma adapt output replace1 replace2
  double z = x + y;
  x = z + y;
#pragma adapt end
  return 0;
}
