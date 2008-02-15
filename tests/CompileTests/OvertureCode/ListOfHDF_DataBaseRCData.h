#ifndef LISTOFHDF_DATABASERCDATA_H
#define LISTOFHDF_DATABASERCDATA_H "ListOfHDF_DataBaseRCData.h"
class HDF_DataBaseRCData;
//
//			ListOfHDF_DataBaseRCData Class Template
//
//  This is the header file for a template class that stores pointers
//  of objects within an array.  The array is dynamic so that only 
//  memory and time limit the number of objects stored on the list.

//
//  Set this parameter to 1 if you find it necessary to track the lists
//  being created and destroyed.
//
#define DEBUGTEMPLATE 0


class ListOfHDF_DataBaseRCData{
  private:

#if DEBUGTEMPLATE
    int theID;		// Unique id for each list object created.
#endif			// the counter static int theIDCount is used
			// to assign a value.

    int listLength;	// Number of elements in the list.
    int memAlloc;	// Current amount of memory allocated for the list.
    HDF_DataBaseRCData **aList;		// Pointer to a list of pointers to object HDF_DataBaseRCData.
    
    void checkRange(int) const;	// Internal range check function.
    
  public:

#if DEBUGTEMPLATE		// Should be initialized for debugging.
    static int theIDCount;	// example:
#endif				//   int ListOfHDF_DataBaseRCData<stuff>::theIDCount = 0;

				// Constructors/Destructors
    ListOfHDF_DataBaseRCData();			//   Default constructor.
    ListOfHDF_DataBaseRCData(const ListOfHDF_DataBaseRCData&);		//   Copy constructor.
   ~ListOfHDF_DataBaseRCData();			//   Destructor.
    
    ListOfHDF_DataBaseRCData& operator=(const ListOfHDF_DataBaseRCData&);	// Equal operator (only pointers of
					// list objects copied).

//  void Iterator( void (HDF_DataBaseRCData::*Function)());  // Function iterator.


			// ListOfHDF_DataBaseRCData Management Functions

    void addElement(HDF_DataBaseRCData &X);		// Add an object to the list.

    void addElement(HDF_DataBaseRCData &X, int index);	// Add an object to the list at a
					//   given location.
    int getLength() const {return listLength;};// Get length of list.

    HDF_DataBaseRCData* getElementPtr(int index) const;	// Get the pointer of the object
					//   at a given location.
    HDF_DataBaseRCData& getElement(int index) const;		// Reference the object at a
    					//   a given location.
    HDF_DataBaseRCData& operator[](int index) const;		// Reference the object at a
    					//   given location.
    void deleteElement(HDF_DataBaseRCData &X);		// Find an element on the list
					//   and delete it.
    void deleteElement(int index);      // Delete the object at a given
					//   location in the list.
    void setElementPtr(HDF_DataBaseRCData *X, int i);    // Set the list element at i
    					//   to point to X.
    void swapElements(int i, int j);    // Swap two elements for sorting
    					//   among other things.
    void clean();			// Deallocate pointer list but
					//   not objects in the list.
    void deepClean();			// Deallocate pointer list and
					//   the pointers on the list.
};

#endif
