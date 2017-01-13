const int a=10;
int foo(int);

main(){
   int x=1;
   foo(x);
}

int foo(int x){
  return x*a;
}
