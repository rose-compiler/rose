//
// simple_prime.C
//
// Simple test code to feed ROSE.
// Class declaration in simple_prime.h.
//

#include "simple_prime.h"

void number::setVal(int v) { value = v; }

int number::getVal() { return value; }

int main()
{
  number i;

  i.setVal(3);

  cout << "i.getVal() = " << i.getVal() << endl;

  return 0;

}
