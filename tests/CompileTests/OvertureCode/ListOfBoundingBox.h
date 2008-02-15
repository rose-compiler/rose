#ifndef LISTOFBOUNDINGBOX_H
#define LISTOFBOUNDINGBOX_H "ListOfBoundingBox.h"
class BoundingBox;
//
//			ListOfBoundingBox Class Template
//
//  This is the header file for a template class that stores pointers
//  of objects within an array.  The array is dynamic so that only 
//  memory and time limit the number of objects stored on the list.

//
//  Set this parameter to 1 if you find it necessary to track the lists
//  being created and destroyed.
//
#define DEBUGTEMPLATE 0


class ListOfBoundingBox{
  private:

#if DEBUGTEMPLATE
    int theID;		// Unique id for each list object created.
#endif			// the counter static int theIDCount is used
			// to assign a value.

    int listLength;	// Number of elements in the list.
    int memAlloc;	// Current amount of memory allocated for the list.
    BoundingBox **aList;		// Pointer to a list of pointers to object BoundingBox.
    
    void checkRange(int) const;	// Internal range check function.
    
  public:

#if DEBUGTEMPLATE		// Should be initialized for debugging.
    static int theIDCount;	// example:
#endif				//   int ListOfBoundingBox<stuff>::theIDCount = 0;

				// Constructors/Destructors
    ListOfBoundingBox();			//   Default constructor.
    ListOfBoundingBox(const ListOfBoundingBox&);		//   Copy constructor.
   ~ListOfBoundingBox();			//   Destructor.
    
    ListOfBoundingBox& operator=(const ListOfBoundingBox&);	// Equal operator (only pointers of
					// list objects copied).

//  void Iterator( void (BoundingBox::*Function)());  // Function iterator.


			// ListOfBoundingBox Management Functions

    void addElement(BoundingBox &X);		// Add an object to the list.

    void addElement(BoundingBox &X, int index);	// Add an object to the list at a
					//   given location.
    int getLength() const {return listLength;};// Get length of list.

    BoundingBox* getElementPtr(int index) const;	// Get the pointer of the object
					//   at a given location.
    BoundingBox& getElement(int index) const;		// Reference the object at a
    					//   a given location.
    BoundingBox& operator[](int index) const;		// Reference the object at a
    					//   given location.
    void deleteElement(BoundingBox &X);		// Find an element on the list
					//   and delete it.
    void deleteElement(int index);      // Delete the object at a given
					//   location in the list.
    void setElementPtr(BoundingBox *X, int i);    // Set the list element at i
    					//   to point to X.
    void swapElements(int i, int j);    // Swap two elements for sorting
    					//   among other things.
    void clean();			// Deallocate pointer list but
					//   not objects in the list.
    void deepClean();			// Deallocate pointer list and
					//   the pointers on the list.
};

#endif
