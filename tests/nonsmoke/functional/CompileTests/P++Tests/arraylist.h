// Allow repeated includes of descriptor.h without error
#ifndef _PPP_ARRAYLIST_H
#define _PPP_ARRAYLIST_H

// GNU will build intances of all objects in the header file if this
// is not specified.  The result is very large object files (too many symbols)
// so we can significantly reduce the size of the object files which will
// build the library (factor of 5-10).
#ifdef GNU
#pragma interface
#endif


#define DEBUGTEMPLATE 0





#define DOUBLEARRAY
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

class List_Of_doubleArray
   {
  protected:

#if DEBUGTEMPLATE
    int theID;		// Unique id for each list object created.
			// the counter static int theIDCount is used
			// to assign a value.
#endif

    int listLength;	// Number of elements in the list.
    int memAlloc;	// Current amount of memory allocated for the list.
    doubleArray  **aList;	// Pointer to a list of pointers to object T.

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
    List_Of_doubleArray();		//   Default constructor.
    List_Of_doubleArray(const List_Of_doubleArray &);		//   Copy constructor.
   ~List_Of_doubleArray();		//   Destructor.
    
    List_Of_doubleArray & operator=(const List_Of_doubleArray &);	// Equal operator (only pointers of
					// list objects copied).

//  void Iterator( void (doubleArray::*Function)());  // Function iterator.


 // List Management Functions

    void addElement(const doubleArray & X);	    // Add an object to the list.

    void addElement( const doubleArray & X, int i ); // Add an object to the list at a
				            //   given location.
    int getLength() const
           {return listLength;};            // Get length of list.

    doubleArray* getElementPtr(int i );	            // Get the pointer of the object
					    //   at a given location.
    doubleArray& getElement(int i ) const;           // Reference the object at a
    					    //   a given location.
    doubleArray& operator[](int i ) const;           // Reference the object at a
    					    //   given location.
    void deleteElement( doubleArray & X );           // Find an element on the list
					    //   and delete it.
    void deleteElement( int i  );           // Delete the object at a given
					    //   location in the list.
    void setElementPtr( doubleArray *X, int i );     // Set the list element at i
    					    //   to point to X.
    void swapElements( int i, int j );      // Swap two elements for sorting
    					    //   among other things.
    void clean();			    // Deallocate pointer list but
					    //   not objects in the list.
    void deepClean();			    // Deallocate pointer list and
					    //   the pointers on the list.
    int getIndex(const doubleArray & X);

    int checkElement(const doubleArray & X);         // Check to see if element is 
					    //   already in the list.
   };


#undef DOUBLEARRAY

#define FLOATARRAY
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

class List_Of_floatArray
   {
  protected:

#if DEBUGTEMPLATE
    int theID;		// Unique id for each list object created.
			// the counter static int theIDCount is used
			// to assign a value.
#endif

    int listLength;	// Number of elements in the list.
    int memAlloc;	// Current amount of memory allocated for the list.
    floatArray  **aList;	// Pointer to a list of pointers to object T.

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
    List_Of_floatArray();		//   Default constructor.
    List_Of_floatArray(const List_Of_floatArray &);		//   Copy constructor.
   ~List_Of_floatArray();		//   Destructor.
    
    List_Of_floatArray & operator=(const List_Of_floatArray &);	// Equal operator (only pointers of
					// list objects copied).

//  void Iterator( void (floatArray::*Function)());  // Function iterator.


 // List Management Functions

    void addElement(const floatArray & X);	    // Add an object to the list.

    void addElement( const floatArray & X, int i ); // Add an object to the list at a
				            //   given location.
    int getLength() const
           {return listLength;};            // Get length of list.

    floatArray* getElementPtr(int i );	            // Get the pointer of the object
					    //   at a given location.
    floatArray& getElement(int i ) const;           // Reference the object at a
    					    //   a given location.
    floatArray& operator[](int i ) const;           // Reference the object at a
    					    //   given location.
    void deleteElement( floatArray & X );           // Find an element on the list
					    //   and delete it.
    void deleteElement( int i  );           // Delete the object at a given
					    //   location in the list.
    void setElementPtr( floatArray *X, int i );     // Set the list element at i
    					    //   to point to X.
    void swapElements( int i, int j );      // Swap two elements for sorting
    					    //   among other things.
    void clean();			    // Deallocate pointer list but
					    //   not objects in the list.
    void deepClean();			    // Deallocate pointer list and
					    //   the pointers on the list.
    int getIndex(const floatArray & X);

    int checkElement(const floatArray & X);         // Check to see if element is 
					    //   already in the list.
   };


#undef FLOATARRAY

#define INTARRAY
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

class List_Of_intArray
   {
  protected:

#if DEBUGTEMPLATE
    int theID;		// Unique id for each list object created.
			// the counter static int theIDCount is used
			// to assign a value.
#endif

    int listLength;	// Number of elements in the list.
    int memAlloc;	// Current amount of memory allocated for the list.
    intArray  **aList;	// Pointer to a list of pointers to object T.

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
    List_Of_intArray();		//   Default constructor.
    List_Of_intArray(const List_Of_intArray &);		//   Copy constructor.
   ~List_Of_intArray();		//   Destructor.
    
    List_Of_intArray & operator=(const List_Of_intArray &);	// Equal operator (only pointers of
					// list objects copied).

//  void Iterator( void (intArray::*Function)());  // Function iterator.


 // List Management Functions

    void addElement(const intArray & X);	    // Add an object to the list.

    void addElement( const intArray & X, int i ); // Add an object to the list at a
				            //   given location.
    int getLength() const
           {return listLength;};            // Get length of list.

    intArray* getElementPtr(int i );	            // Get the pointer of the object
					    //   at a given location.
    intArray& getElement(int i ) const;           // Reference the object at a
    					    //   a given location.
    intArray& operator[](int i ) const;           // Reference the object at a
    					    //   given location.
    void deleteElement( intArray & X );           // Find an element on the list
					    //   and delete it.
    void deleteElement( int i  );           // Delete the object at a given
					    //   location in the list.
    void setElementPtr( intArray *X, int i );     // Set the list element at i
    					    //   to point to X.
    void swapElements( int i, int j );      // Swap two elements for sorting
    					    //   among other things.
    void clean();			    // Deallocate pointer list but
					    //   not objects in the list.
    void deepClean();			    // Deallocate pointer list and
					    //   the pointers on the list.
    int getIndex(const intArray & X);

    int checkElement(const intArray & X);         // Check to see if element is 
					    //   already in the list.
   };


#undef INTARRAY

#endif  /* !defined(_PPP_ARRAYLIST_H) */






