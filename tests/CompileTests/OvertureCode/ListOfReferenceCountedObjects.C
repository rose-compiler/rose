#include "ListOfReferenceCountedObjects.h"
#include "wdhdefs.h"
#include <assert.h>

//  ---Default constructor---
template <class T> 
ListOfReferenceCountedObjects<T>::
ListOfReferenceCountedObjects()
{
  initialize();  
}

// ---Create a list with a given number of elements---
template <class T> 
ListOfReferenceCountedObjects<T>::
ListOfReferenceCountedObjects( const int numberOfElements )
{
  initialize();  
  for( int i=0; i<numberOfElements; i++ )
    addElement();
  
}

// ---Copy constructor :  deep copy by default---
template <class T> 
ListOfReferenceCountedObjects<T>::
ListOfReferenceCountedObjects( const ListOfReferenceCountedObjects<T> & X, const CopyType copyType )
{
  switch (copyType)
  {
  case DEEP:
    initialize();
    (*this)=X;
    break;
  case SHALLOW:
    rcData=X.rcData;   // before we call reference we must make rcData point to something
    rcData->incrementReferenceCount();
    reference( X ); 
    break;
  case NOCOPY:
    initialize();
    break;
  }
}

template <class T>
void ListOfReferenceCountedObjects<T>::
initialize()
{
  rcData = ::new RCData;
  rcData->incrementReferenceCount();
  rcData->listLength = 0;
  rcData->memAlloc = 0;
  rcData->aList    = NULL;
}


// --- Destructor---
template <class T> 
ListOfReferenceCountedObjects<T>::
~ListOfReferenceCountedObjects()
{
//  cout << "destructor called for ListOfReferenceCountedData "<<
//    " referenceCount = " << rcData->getReferenceCount()-1 << endl;
  
  if( rcData->decrementReferenceCount() == 0 )
    deleteStuff();
}


template <class T>
void ListOfReferenceCountedObjects<T>::
deleteStuff()
{
  for( int i=0; i<listLength(); i++)
    ::delete rcData->aList[i];      // first delete the objects
  
  if( rcData->memAlloc > 0 )
    ::delete [] rcData->aList;       // delete array of pointers
  ::delete rcData;
}


// ---Assignment with = is a deep copy ---
template <class T> 
ListOfReferenceCountedObjects<T> & ListOfReferenceCountedObjects<T>::
operator=(const ListOfReferenceCountedObjects<T> &X)
{

  // make the list the correct size
  int i;
  int length=listLength();
  if( length <  X.listLength() )
  {
    for( int i=length; i<X.listLength(); i++ )
      addElement();
  }
  else if( length > X.listLength() )
  {
    for( i=length-1; i >= X.getLength(); i-- )
      deleteElement();
  }
  if( rcData->listLength != X.listLength() )
  {
    cout << "ListOfReferenceCountedObjects:ERROR in operator=" << endl;
    cout << "         listLength's are not equal " << endl;
  }    

  for( i=0; i<listLength(); i++)
    (*this)[i] = X[i];  // this is assumed to be a deep copy

  return *this;
}

// ---Reference this list to another list---
template <class T>
void ListOfReferenceCountedObjects<T>::
reference( const ListOfReferenceCountedObjects<T> & list )
{
  if( rcData->decrementReferenceCount() == 0 )
    deleteStuff();  
  rcData=list.rcData;
  rcData->incrementReferenceCount();

}

// ---break the reference of this list (if any)---
template <class T>
void ListOfReferenceCountedObjects<T>::
breakReference()
{
  // if there is only 1 reference, there is no need to make a new copy  
  if( rcData->getReferenceCount() != 1 )
  {
    ListOfReferenceCountedObjects<T> list = *this;  // makes a deep copy
    reference(list);   // make a reference to this new copy
  }
}

template <class T>
void ListOfReferenceCountedObjects<T>::
destroy()
// =======================================================================
// /Description:
//    Destroy the list. 
// =======================================================================
{
  if( rcData->decrementReferenceCount() == 0 )
  {
    // delete all the data if no-one is referencing it.
    deleteStuff();
  }
  // make this a valid null list
  initialize();
}

template <class T> 
void ListOfReferenceCountedObjects<T>::
openAPositionForAnElement( const int index )
// =================================================================================================
// /Description: (private routine)
//   Adjust the list (if necessary) so that a new Element can be created at the "index" position
//   Whoever calls this function is responsible for "newing" a T and adding it to the list
// =================================================================================================
{

  // If there is enough memory just add it in!
  if( index==listLength() && listLength() < rcData->memAlloc )
  {  
    rcData->listLength++;
//    rcData->aList[rcData->listLength++]= ::new T;
//    assert(rcData->aList[rcData->listLength-1]!=NULL);
    return;
  }

  if(listLength() >= rcData->memAlloc)
  {
    if(rcData->memAlloc ==0  )			// Double the memory if it less than
      rcData->memAlloc  = 2;			// 100, otherwise increase by 10 percent
    else if(rcData->memAlloc < 100)
      rcData->memAlloc *= 2;
    else
      rcData->memAlloc += rcData->memAlloc/10;

    T **aListTmp;

    aListTmp = ::new (T* [rcData->memAlloc]); // create an array of pointers
    assert(aListTmp!=NULL);
    
    for(int i=0; i<listLength(); i++)
      aListTmp[i] = rcData->aList[i];

    ::delete [] rcData->aList;                // delete old list
    rcData->aList = aListTmp;
  }
  
  rcData->listLength++;				// Add in the object by ...
  checkRange(index);
    
  for(int i=listLength()-1; i>index; i--)   // Displacing  elements.
    rcData->aList[i] = rcData->aList[i-1];

//  rcData->aList[index] = ::new T;			// Put it at the desired location.
//  assert(rcData->aList[index]!=NULL);
}



// ---Add a new object at position index---
// note: addElement creates a new object
template <class T> 
void ListOfReferenceCountedObjects<T>::
addElement( const int index )
{
  openAPositionForAnElement( index );     
  rcData->aList[index]= ::new T;
  assert(rcData->aList[index]!=NULL);

}

//--- Add a new object to the end of the list---
template <class T> 
void ListOfReferenceCountedObjects<T>::
addElement( )
{
  addElement( listLength() );
}


// ---Add a new object at position index and reference to Object t---
template <class T> 
void ListOfReferenceCountedObjects<T>::
addElement( const T & t, const int index )
{
//  addElement( index );
//  (*this)[index].reference(t);

  openAPositionForAnElement( index );     
  // do a shallow copy as this will be faster
// *wdh   rcData->aList[index]= ::new T(t,SHALLOW);  // trouble with A++ arrays
// *****
  rcData->aList[index]= ::new T;
  rcData->aList[index]->reference(t);
// *****
  assert(rcData->aList[index]!=NULL);
}  

// ---Add a new object at the end of the list and reference to Object t---
template <class T> 
void ListOfReferenceCountedObjects<T>::
addElement( const T & t )
{
  addElement( t,listLength() );
}

//  ---Delete a given element ---
template <class T> 
void ListOfReferenceCountedObjects<T>::
deleteElement(const int index)
{
  checkRange(index);  // Check to make sure index is in the current range.
  
  ::delete rcData->aList[index];				    
  for(int i=index; i<listLength()-1; i++)
    rcData->aList[i] = rcData->aList[i+1];
  
  rcData->listLength--;
}

// ---Delete the last element ---
template <class T> 
void ListOfReferenceCountedObjects<T>::
deleteElement()
{
  deleteElement( listLength()-1 );
}

//		Delete an element with the same pointer
template <class T> 
void ListOfReferenceCountedObjects<T>::
deleteElement( const T &X)
{
  for(int i=0; i<listLength(); i++)
  {	
    if(&X == rcData->aList[i])
    {
      deleteElement(i);
      return;
    }
  }
					// Not there!
  cerr << "ListOfReferenceCountedObjects: Object not found in list for deletion!" << endl;
  cerr << "Proceed with caution..." << endl; 
}

// Swap two elements for sorting among other things.
template <class T> void 
ListOfReferenceCountedObjects<T>::
swapElements( const int i, const int j)
{
  checkRange(i);
  checkRange(j);
    
  T *tmp;
  tmp = rcData->aList[i];
  rcData->aList[i] = rcData->aList[j];
  rcData->aList[j] = tmp;
}


//  Get an object element by reference
template <class T> 
T& ListOfReferenceCountedObjects<T>::
operator[]( const int index ) const 
{
  checkRange(index);  
  return *rcData->aList[index];
}
    
#undef getIndex
// Return the index of an object, return -1 if not found
template <class T> 
int ListOfReferenceCountedObjects<T>::
getIndex(const T & X) const
{
  for(int i=0; i<listLength(); i++)
  {	
    if(&X == rcData->aList[i])
    {
      return i;
    }
  }
  return -1;  // not found
}


//		Internal range check routine
template <class T> 
void ListOfReferenceCountedObjects<T>::
checkRange( const int index) const 
{
  if(index < 0 || index > listLength() - 1)
  {
    cerr << "ListOfReferenceCountedObjects: List Index Out of Range!" << endl;
    cerr << "  trying to reference a list with index=" << index << endl;
    cerr << "  but the list only has indicies in the range: [0," << listLength()-1 << "]" << endl;
    cerr << "  length of List = " << getLength() << endl;
    throw "ListOfReferenceCountedObjects: List Index Out of Range!";
  }  
}

