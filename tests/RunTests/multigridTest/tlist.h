#ifndef TLIST_H
#define TLIST_H "tlist.h"

//
//			List Class Template
//
//  This is the header file for a template class that stores pointers
//  of objects within an array.  The array is dynamic so that only 
//  memory and time limit the number of objects stored on the list.

//
//  Set this parameter to 1 if you find it necessary to track the lists
//  being created and destroyed.
//
#define DEBUGTEMPLATE FALSE

template<class T>
class List{
  private:

#if DEBUGTEMPLATE
    int theID;		// Unique id for each list object created.
#endif			// the counter static int theIDCount is used
			// to assign a value.

    int listLength;	// Number of elements in the list.
    int memAlloc;	// Current amount of memory allocated for the list.
    T **aList;		// Pointer to a list of pointers to object T.
    
    void checkRange(int) const;	// Internal range check function.
    
  public:

#if DEBUGTEMPLATE		// Should be initialized for debugging.
    static int theIDCount;	// example:
#endif				//   int List<stuff>::theIDCount = 0;

				// Constructors/Destructors
    List();			//   Default constructor.
    List(const List&);		//   Copy constructor.
   ~List();			//   Destructor.
    
    List& operator=(const List&);	// Equal operator (only pointers of
					// list objects copied).

//  void Iterator( void (T::*Function)());  // Function iterator.


			// List Management Functions

    void addElement( const T &X);		// Add an object to the list.

    void addElement( const T &X, int index);	// Add an object to the list at a
					//   given location.
    void addElement(T* X, int index);	// Add a pointer object to the list at a
					//   given location.
    int getLength() const {return listLength;};// Get length of list.

    T* getElementPtr(int index);	// Get the pointer of the object
					//   at a given location.
    T& getElement(int index);		// Reference the object at a
    					//   a given location.
    T& operator[] (int index) const;	// Reference the object at a
    					//   given location.
    void deleteElement(T &X);		// Find an element on the list
					//   and delete it.
    void deleteElement(int index);      // Delete the object at a given
					//   location in the list.
    void setElementPtr(T *X, int i);    // Set the list element at i
    					//   to point to X.
    void swapElements(int i, int j);    // Swap two elements for sorting
    					//   among other things.
    void clean();			// Deallocate pointer list but
					//   not objects in the list.
    void deepClean();			// Deallocate pointer list and
					//   the pointers on the list.
    void append( const List<T> & X );
};

#include "tlist.C"

#endif











