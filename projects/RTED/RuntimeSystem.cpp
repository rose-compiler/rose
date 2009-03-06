/* -----------------------------------------------------------
 * tps : 6th March 2009: RTED
 * RuntimeSystem called by each transformed source file
 * -----------------------------------------------------------*/
//#include "rose.h"
#include <iostream>
#include <map>
#include <string>

#include "RuntimeSystem.h"

using namespace std;

/* -----------------------------------------------------------
* create array and store its size
* -----------------------------------------------------------*/
void
RuntimeSystem::roseCreateArray(std::string name, int size ){
  cout << "Called : roseCreateArrary : " << name <<
    " , " << size << endl;
  arrays[name]=size;
}

/* -----------------------------------------------------------
* check if array is out of bounds
* -----------------------------------------------------------*/
void
RuntimeSystem::roseArrayAccess(std::string name, int pos){
  map<string,int>::const_iterator it = arrays.find(name);
  if (it!=arrays.end()) {
    int size = it->second;
    cout << "Found array : " << name << " : " << size << ", " << pos << endl;
    if (pos>=size || pos<0)
      cout << "Violation detected." << endl;
  }
}
