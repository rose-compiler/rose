#include <iostream.h>
#include <stdio.h>

#include "list.h"

// Test comment 1.

int main()
{

  list<float> test_list(0); 
  list<float>::iterator itor;

  itor = test_list.begin();
  test_list.insert(itor,(float)3.14);
  cout << "*itor(0) =  " << *--itor << endl;
  itor++;
  test_list.insert(itor,(float)2.71);
  cout << "*itor(1) =  " << *--itor << endl;

  itor = test_list.begin();
  cout << "*itor(0) = " << *itor << endl;
  itor++;
  cout << "*itor(1) = " << *itor << endl;

  cout << "front = " << test_list.front() << endl;
  cout << "back =  " << test_list.back() << endl;

  return 0;

}

