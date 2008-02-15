
#ifndef _LOCALSTL_H
#define _LOCALSTL_H

typedef char Direction;
#define Forward 0
#define Backward 1

#include <string>
#include <list>
#include <map>
#include <set>
#include <bitset>
#include <vector>

#include <iostream>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

#endif /* _LOCALSTL_H */

