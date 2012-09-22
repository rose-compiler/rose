int x;

void f() {
  if(x==2) {
	x=3;
	return;
  }
  if(x==1) {
	x=2;
	return;
  }
  x=10;
  return;
}

int main() {
  int x; // fake
  x=1;
  while(1)
	f();
  x=5;
  return 1;
}
