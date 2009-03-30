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
RuntimeSystem::roseCreateArray(std::string name, int dimension, bool stack, long int sizeA, long int sizeB ){
  cout << ">>> Called : roseCreateArray : " << name << " dim"<< dimension <<
	  " - [" << sizeA << "][" << sizeB << "]" << endl;
  if (dimension==1)
	  arrays1D[name]=sizeA;
  else if (dimension==2) {
	  // check if exist
	  std::map<std::string, Array2D*>::const_iterator it = arrays2D.find(name);
	  Array2D* array =NULL;
	  if (it!=arrays2D.end()) {
		  // array exists
		  array = it->second;
		  long int totalsize = array->size1;
		  if (sizeA<0 || sizeA>=totalsize) {
			  cerr << " Violation detected :  Array too small to allocate more memory " << endl;
		  } else {
			  cerr << " >>> CREATING Array : arr ["<<totalsize<<"]["<<sizeB<<"]"<< "  alloc : ["<<sizeA<<"]="<<sizeB<<endl;
			  array->allocate(sizeA,sizeB); //arr[sizeA][sizeB]
		  }
	  } else {

		  // new array
		  array = new Array2D(sizeA);// ptr [][] = malloc (20)   20 == totalsize
		  arrays2D[name]=array;
	  }
  }
}

/* -----------------------------------------------------------
* check if array is out of bounds
* -----------------------------------------------------------*/
void
RuntimeSystem::roseArrayAccess(std::string name, int posA, int posB, std::string filename, int line){
  cout << "    Called : roseArrayAccess : " << name << " ";
  map<string,int>::const_iterator it = arrays1D.find(name);
  if (it!=arrays1D.end()) {
    int size = it->second;
    cout << "       Found 1Dim array : " << " : " << size << ", " << posA << endl;
    if (posA>=size || posA<0)
      cout << "  >>>>>> Violation detected : Array size: " << size << " accessing: " << posA <<
      "  in : " << filename << "  line: " << line << endl;
  }

  std::map<std::string, Array2D*>::const_iterator it2 = arrays2D.find(name);
  if (it2!=arrays2D.end()) {
	  Array2D* array = it2->second;
	  int sizeA = array->size1;
	  int sizeB = array->getSize(posA);
    cout << "  Found 2Dim array :  size: [" << sizeA << "][" << sizeB << "]  pos: [" << posA << "][" << posB << "]" <<endl;
    if (posA>=sizeA || posA<0 || posB>=sizeB || posB<0)
        cout << "  >>>>>> Violation detected : Array size: [" << sizeA << "]["<<sizeB<<"] accessing: ["  << posA <<
        "][" << posB << "]  in : " << filename << "  line: " << line << endl;
  }
  std::cout << endl;
}
