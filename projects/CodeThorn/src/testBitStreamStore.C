#include "BitStreamStore.h"
#include "BitStreamStore.C"

#include <iostream>

using namespace std;

int main() {
  BitStreamStore bss;
  int x_in=1;
  bss.write(x_in,3);
  cout<<bss.toString()<<endl;
  int y_in=15;
  bss.write(y_in,5);
  cout<<bss.toString()<<endl;

  int pos=bss.lastPos();
  cout<<"Pos:"<<pos<<endl;
  int y_out=bss.readBack(5,pos);
  cout<<y_out<<endl;
  int x_out=bss.readBack(3,pos);
  cout<<x_out<<endl;

  pos=0;
  cout<<"Pos:"<<pos<<endl;
  int x_out2=bss.readFront(3,pos);
  cout<<x_out<<endl;
  int y_out2=bss.readFront(5,pos);
  cout<<y_out<<endl;

  if(x_out==x_out2 && y_out==y_out2) {
	cout << "PASS."<<endl;
	return 0;
  } else {
	cout << "FAIL."<<endl;
	return 1;
  }
}
