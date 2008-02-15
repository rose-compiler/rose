#include <iostream.h>
#include "templates_prime.h"

#define STL 0
#define TLIST 0

#if STL
#include <list>
#include <list.h>
#endif

#if TLIST
#include "tlist.h"
#endif

int main()
{
  number<int> i;
  number<float> f;

#if STL
  list<float> test_list(0); 
  list<float>::iterator itor;
#endif

#if TLIST
  List<float> test_tlist; 
#endif

  i.setVal(3);

  f.setVal(3.14);

  cout << "i = " << i.getVal() << endl;

  cout << "f = " << f.getVal() << endl;

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

  cout << "pi = " << test_tlist.operator[](0) << endl;
  cout << "e  = " << test_tlist.operator[](1) << endl;

  test_tlist.deleteElement(1);
  test_tlist.deleteElement(0);
#endif

  return 0;

}

