//
// simple.C
//
// Simple test code to feed ROSE.
//

#include <iostream.h>

class number {

  int value;

public:

  int getVal(); 

  void setVal(int v);

};

void number::setVal(int v) { value = v; }

int number::getVal() { return value; }

int main()
{
  number i;

  i.setVal(3);

  cout << "i.getVal() = " << i.getVal() << endl;

  return 0;

}
