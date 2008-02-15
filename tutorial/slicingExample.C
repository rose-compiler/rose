/* 
  Example for the tutorial
*/

int add(int a, int b);
int inc(int z);

int main(){

  int x;
  int y = 5;
  int z;
  int a = 8;
  int b = 7;

  x = 4;
  z = 5;
  b = 2;

  int i = add(x,y);
  if(i<=y+40 && y!=a){
    b = b + x*x;
    x = x*i;
    z = z +  x;
    a = a + x;  
    b = a;
  }

  for(int j=0; j<=a; j++){
    z = z+j;
  }
  
#pragma start_slicing_criterion
  y = x + b;
#pragma end_slicing_criterion
  y = add(x,y);
  
  return 0;
}


int add(int a, int b){
  int c;
  c = a + b;
  return c;
}

int inc(int z){
  z = add(z,1);
  return z;
}
