#include <iostream.h>
#include <stdio.h>

#include "tlist.h"

// Test comment 1.

int main()
{
  List<float> test_tlist; 

  test_tlist.addElement(3.14);
  test_tlist.addElement(2.71);

  cout << "pi = " << test_tlist[0] << endl;
  cout << "e  = " << test_tlist[1] << endl;

  test_tlist.deleteElement(1);
  test_tlist.deleteElement(0);

  return 0;

}

