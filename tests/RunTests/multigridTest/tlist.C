#include <iostream.h>
#include <stdlib.h>

//#include "tlist.h"

#if DEBUGTEMPLATE	// Should be initialized for debugging.
template<class T>
int List<T>::theIDCount;	// example:
#endif				//   int List<stuff>::theIDCount = 0;

//		Default constructor

template<class T> List<T>::List(){

#if DEBUGTEMPLATE
  theIDCount++;
  theID = theIDCount;
  cout << "List default constructor called.  ";
  cout << "List #" << theID << " created." << endl;
#endif
  
  listLength = 0;
  aList    = 0;
  memAlloc = 0;
}

//		Copy constructor

template<class T> List<T>::List(const List<T> &X){

#if DEBUGTEMPLATE			// Assign a unique ID from the
  theIDCount++;				// static counter.
  theID =theIDCount;
  cout << "List copy constructor called.     ";
  cout << "ID " << X.theID <<" copied to " << theID << "." << endl;
#endif

  listLength = X.listLength;
  
  if(listLength < 1){			// Determine if any allocation is
    aList    = 0;			//   necessary.
    memAlloc = 0;
    return;
  }
  
  aList = new (T* [listLength]);		// Allocate pointer space.
  memAlloc = listLength;
  
  for(int i=0; i<listLength; i++)		// Copy pointers.
    aList[i] = X.aList[i];
}

//		Destructor

template<class T> List<T>::~List(){

#if DEBUGTEMPLATE
  cout << "List destructor called.           ";
  cout << "List #" << theID << " destroyed." << endl;
#endif
  
  delete[] aList;				// Delete the list of pointers (only).
}

//		Class equal operator

template<class T> List<T>& List<T>::operator=(const List<T> &X){

#if DEBUGTEMPLATE
  cout << "List = operator called.           ";
  cout << "ID " << X.theID <<" copied to " << theID << "." << endl;
#endif

  listLength = X.listLength;		// Like the copy constructor
  delete[] aList;			//   allocate and copy pointer list.
  
  if(listLength < 1){
    aList    = 0;
    memAlloc = 0;
    return *this;
  }
  
  aList = new (T* [listLength]);
  memAlloc = listLength;
  
  for(int i=0; i<listLength; i++)
    aList[i] = X.aList[i];

  return *this;
}

//		Function Iterator

/*
template<class T> void List<T>::Iterator(void (T::*Function)()){

  for(int i=0; i<listLength; i++)  // works for nontemplate.
    (aList[i]->*Function)();
} 
*/

//		Add an object element to the list.

template<class T> void List<T>::addElement( const T &X){

  if(listLength < memAlloc){	// If there is enough memory just add it in!
    aList[listLength++] = &((T&)X);
    return;
  }
  
  if(memAlloc ==0  )		// Double the memory size if it is less then
    memAlloc  = 2;		//   100 otherwise increase by 10 percent.
  else if(memAlloc < 100)
    memAlloc *= 2;
  else
    memAlloc += memAlloc/10;

  T **aListTmp;

  aListTmp = new (T* [memAlloc]);
// assert (aListTmp != NULL);

  for(int i=0; i<listLength; i++){  	// Copy object pointers into new space.
    aListTmp[i] = aList[i];
  }

  delete[] aList;			// Delete old list and add the object.
  aList = aListTmp;
// assert (aList != NULL);

  aList[listLength++] = &((T&)X);
}

template<class T> void List<T>::addElement( const T &X, int index){

  if(listLength >= memAlloc){
  
    if(memAlloc ==0  )			// Like above increase array size
      memAlloc  = 2;			//   if necessary.
    else if(memAlloc < 100)
      memAlloc *= 2;
    else
      memAlloc += memAlloc/10;

    T **aListTmp;

    aListTmp = new (T* [memAlloc]);
    
    for(int i=0; i<listLength; i++){
      aListTmp[i] = aList[i];
    }

    delete[] aList;
    aList = aListTmp;
  }
  
  listLength++;				// Add in the object by ...
  checkRange(index);
    
  for(int i=listLength-1; i>index; i--){  // Displacing  elements.
    aList[i] = aList[i-1];
  }
  
  aList[index] = &X;			// Put it at the desired location.
}


// We need this function because the passing of the data through the "T& X" 
// strips off the derived type information and later forces a call to the base 
// class's virtual function (which could be pure, but in any case results in a 
// subtle error).
template<class T> void List<T>::addElement(T* Xptr, int index){

  if(listLength >= memAlloc){
  
    if(memAlloc ==0  )			// Like above increase array size
      memAlloc  = 2;			//   if necessary.
    else if(memAlloc < 100)
      memAlloc *= 2;
    else
      memAlloc += memAlloc/10;

    T **aListTmp;

    aListTmp = new (T* [memAlloc]);
    
    for(int i=0; i<listLength; i++){
      aListTmp[i] = aList[i];
    }

    delete[] aList;
    aList = aListTmp;
  }
  
  listLength++;				// Add in the object by ...
  checkRange(index);
    
  for(int i=listLength-1; i>index; i--){  // Displacing  elements.
    aList[i] = aList[i-1];
  }
  
  aList[index] = Xptr;			// Put it at the desired location.
}

//		Delete an element a location

template<class T> void List<T>::deleteElement(int index){

  checkRange(index);			// Check to make sure index is in the
					//   current range.
  for(int i=index; i<listLength-1; i++)
    aList[i] = aList[i+1];
  
  listLength--;
}

//		Delete an element with the same pointer

template<class T> void List<T>::deleteElement(T &X){

  for(int i=0; i<listLength; i++){	// Loop until you find it.
    if(&X == aList[i]){
      deleteElement(i);
      return;
    }
  }
					// Not there!

  cerr << "Object not found in list for deletion!" << endl;
  cerr << "Proceed with caution..." << endl; 
  abort();
}

//		Swap two elements for sorting among other things.

template<class T> void List<T>::swapElements(int i, int j){

  checkRange(i);
  checkRange(j);
    
  T *tmp;
  
  tmp      = aList[i];
  aList[i] = aList[j];
  aList[j] = tmp;
}

//		Set the list element at i to point to X.
					
template<class T> void List<T>::setElementPtr(T *X, int index){
  checkRange(index);
  aList[index] = X;
}

//		Get an object element by reference (the cute way)

template<class T> T& List<T>::operator[](int index) const {
  checkRange(index);
// cout << "Inside of List<T>::operator[](int index)" << endl;
// assert (aList != NULL);
  return *aList[index];
}
    
//		Get an object element by reference

template<class T> T& List<T>::getElement(int index){
  checkRange(index);  
  return *aList[index];
}

//		Get an object element pointer

template<class T> T *List<T>::getElementPtr(int index){
  
  checkRange(index);
  return aList[index];
}

//		Deallocate the list memory and set length to zero.

template<class T> void List<T>::clean(){
  listLength = 0;
}

//		Deallocate the list memory and delete the objects.

template<class T> void List<T>::deepClean(){
  for(int i=0; i<listLength; i++)
    delete aList[i];
  
  listLength = 0;
}

//		Internal range check routine

template<class T> void List<T>::checkRange(int index) const {
  if(index < 0 || index > listLength - 1){
    cerr << "List Index Out of Range!" << endl;
    cerr << "  Index Value: " << index << endl;
    cerr << "  Index Range: 0 - " << listLength-1 << endl;
    abort();
  }  
}

template<class T> void List<T>::append( const List<T> & X )
   {
     for(int i=0; i < X.listLength; i++)
        {
          addElement(*X.aList[i]);
          checkRange(i);
        }
   }



