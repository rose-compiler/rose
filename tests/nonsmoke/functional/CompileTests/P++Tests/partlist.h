// Allow repeated includes of descriptor.h without error
#ifndef _PPP_PARTITION_OBJECT_LIST_H
#define _PPP_PARTITION_OBJECT_LIST_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif


#define DEBUGTEMPLATE 0





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

class List_Of_Partitioning_Type
   {
  protected:

#if DEBUGTEMPLATE
    int theID;		// Unique id for each list object created.
			// the counter static int theIDCount is used
			// to assign a value.
#endif

    int listLength;	// Number of elements in the list.
    int memAlloc;	// Current amount of memory allocated for the list.
    Partitioning_Type  **aList;	// Pointer to a list of pointers to object T.

 // Records multiple references to the list class
    int referenceCount;
    
    void checkRange(int) const;	// Internal range check function.
    
  public:

#if DEBUGTEMPLATE
 // Should be initialized for debugging. 
 // example:
 // int List<stuff>::theIDCount = 0;
    static int theIDCount;
#endif

				// Constructors/Destructors
    List_Of_Partitioning_Type();		//   Default constructor.
    List_Of_Partitioning_Type(const List_Of_Partitioning_Type &);		//   Copy constructor.
   ~List_Of_Partitioning_Type();		//   Destructor.
    
    List_Of_Partitioning_Type & operator=(const List_Of_Partitioning_Type &);	// Equal operator (only pointers of
					// list objects copied).

//  void Iterator( void (Partitioning_Type::*Function)());  // Function iterator.


 // List Management Functions

    void addElement(const Partitioning_Type & X);	    // Add an object to the list.

    void addElement( const Partitioning_Type & X, int i ); // Add an object to the list at a
				            //   given location.
    int getLength() const
           {return listLength;};            // Get length of list.

    Partitioning_Type* getElementPtr(int i );	            // Get the pointer of the object
					    //   at a given location.
    Partitioning_Type& getElement(int i ) const;           // Reference the object at a
    					    //   a given location.
    Partitioning_Type& operator[](int i ) const;           // Reference the object at a
    					    //   given location.
    void deleteElement( Partitioning_Type & X );           // Find an element on the list
					    //   and delete it.
    void deleteElement( int i  );           // Delete the object at a given
					    //   location in the list.
    void setElementPtr( Partitioning_Type *X, int i );     // Set the list element at i
    					    //   to point to X.
    void swapElements( int i, int j );      // Swap two elements for sorting
    					    //   among other things.
    void clean();			    // Deallocate pointer list but
					    //   not objects in the list.
    void deepClean();			    // Deallocate pointer list and
					    //   the pointers on the list.
    int getIndex(const Partitioning_Type & X);

    int checkElement(const Partitioning_Type & X);         // Check to see if element is 
					    //   already in the list.
   };



#endif  /* !defined(_PPP_PARTITION_OBJECT_LIST_H) */






