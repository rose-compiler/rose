#ifndef LISTOFCOMPOSITEGRIDOPERATORS_H
#define LISTOFCOMPOSITEGRIDOPERATORS_H "ListOfCompositeGridOperators.h"
class CompositeGridOperators;
//
//			ListOfCompositeGridOperators Class Template
//
//  This is the header file for a template class that stores pointers
//  of objects within an array.  The array is dynamic so that only 
//  memory and time limit the number of objects stored on the list.

//
//  Set this parameter to 1 if you find it necessary to track the lists
//  being created and destroyed.
//
#define DEBUGTEMPLATE 0


class ListOfCompositeGridOperators{
  private:

#if DEBUGTEMPLATE
    int theID;		// Unique id for each list object created.
#endif			// the counter static int theIDCount is used
			// to assign a value.

    int listLength;	// Number of elements in the list.
    int memAlloc;	// Current amount of memory allocated for the list.
    CompositeGridOperators **aList;		// Pointer to a list of pointers to object CompositeGridOperators.
    
    void checkRange(int) const;	// Internal range check function.
    
  public:

#if DEBUGTEMPLATE		// Should be initialized for debugging.
    static int theIDCount;	// example:
#endif				//   int ListOfCompositeGridOperators<stuff>::theIDCount = 0;

				// Constructors/Destructors
    ListOfCompositeGridOperators();			//   Default constructor.
    ListOfCompositeGridOperators(const ListOfCompositeGridOperators&);		//   Copy constructor.
   ~ListOfCompositeGridOperators();			//   Destructor.
    
    ListOfCompositeGridOperators& operator=(const ListOfCompositeGridOperators&);	// Equal operator (only pointers of
					// list objects copied).

//  void Iterator( void (CompositeGridOperators::*Function)());  // Function iterator.


			// ListOfCompositeGridOperators Management Functions

    void addElement(CompositeGridOperators &X);		// Add an object to the list.

    void addElement(CompositeGridOperators &X, int index);	// Add an object to the list at a
					//   given location.
    int getLength() const {return listLength;};// Get length of list.

    CompositeGridOperators* getElementPtr(int index) const;	// Get the pointer of the object
					//   at a given location.
    CompositeGridOperators& getElement(int index) const;		// Reference the object at a
    					//   a given location.
    CompositeGridOperators& operator[](int index) const;		// Reference the object at a
    					//   given location.
    void deleteElement(CompositeGridOperators &X);		// Find an element on the list
					//   and delete it.
    void deleteElement(int index);      // Delete the object at a given
					//   location in the list.
    void setElementPtr(CompositeGridOperators *X, int i);    // Set the list element at i
    					//   to point to X.
    void swapElements(int i, int j);    // Swap two elements for sorting
    					//   among other things.
    void clean();			// Deallocate pointer list but
					//   not objects in the list.
    void deepClean();			// Deallocate pointer list and
					//   the pointers on the list.
};

#endif
