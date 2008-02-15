#include <iostream.h>
#include "templates_prime.h"

int main()
{
  number<int> i;
  number<float> f;

  i.setVal(3);

  f.setVal(3.14);

  cout << "i = " << i.getVal() << endl;

  cout << "f = " << f.getVal() << endl;

  return 0;

}

