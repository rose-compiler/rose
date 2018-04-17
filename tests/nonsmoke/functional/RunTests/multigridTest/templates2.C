#include <iostream.h>
#include <stdio.h>

#define STL 0
#define TLIST 1

#if STL
#include <list.h>
#endif

#if TLIST
 #include "tlist.h"
#endif

int main()
{

#if STL
  list<float> test_list(0); 
  list<float>::iterator itor;
#endif

#if TLIST
  List<float> test_tlist; 
#endif

// Test comment 1. 
// Test comment 2. 

#if STL
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
#endif

#if TLIST
  test_tlist.addElement(3.14);
  test_tlist.addElement(2.71);

  cout << "pi = " << test_tlist[0] << endl;
  cout << "e  = " << test_tlist[1] << endl;

  test_tlist.deleteElement(1);
  test_tlist.deleteElement(0);
#endif

  return 0;

}

