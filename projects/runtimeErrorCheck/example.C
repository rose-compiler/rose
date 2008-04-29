#include <iostream>

using namespace std;

int main(int argc, char** argv) {

  int* x= new int[2];

  x[0]=1;
  x[1]=2;
  x[2]=3;

  int y=x[5];
  // code that should be added
  if (x[0]==NULL) { cerr << " x[0]==NULL " <<endl; exit(0);}
  if (y==NULL) { cerr << " y==NULL " <<endl; exit(0);}
  int a=x[0]/y;
}
