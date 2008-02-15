/*
This file is for testing do-while constructions.
*/
int printf(const char *, ...);

int main(){

  int a =5;
  int b=5;
  int counter = 1;
  do{
    counter++;
    b=a+5;
  }while(counter<=a);

  return 0;
}

